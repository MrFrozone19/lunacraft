//
// Port of Minecraft's MapGenCaves as it was before the 1.7 cave nerf (Beta 1.7.3 - Release 1.6.4).
//
// Caves are carved "worm" style: each tunnel starts at a random point, walks forward with a smoothly
// drifting yaw/pitch, carves an ellipsoid at every step, and splits into two tunnels partway through.
// Every source chunk within RANGE chunks of the target simulates its own caves and only the parts that
// pass through the target are carved, so neighboring chunks line up without needing each other's data.
//
// Minecraft's chunks are 16x16, so a Lunacraft chunk is carved as 2x2 Minecraft-sized chunks. This keeps
// cave density and spacing identical to the original. Java's Random and Minecraft's sine table are
// reproduced so the random walk behaves the same way.
//

#include <array>
#include <cmath>

#include "cave_generation.h"
#include "constants.h"
#include "helpers.h"

namespace
{
    constexpr int MC_CHUNK_SIZE = 16;
    constexpr int RANGE = 8;          // Source chunks considered in each direction
    constexpr int MAX_CARVE_Y = 120;  // Matches Minecraft's clamp
    constexpr int LAVA_LEVEL = 10;    // Minecraft fills below this with lava (Lunacraft has none)
    constexpr float PI = 3.1415927f;

    static_assert(CHUNK_SIZE % MC_CHUNK_SIZE == 0, "Chunk size must be a multiple of 16 for cave generation");

    // java.util.Random
    class JavaRandom
    {
        private:
            static constexpr uint64_t MULTIPLIER = 0x5DEECE66Dull;
            static constexpr uint64_t MASK = (1ull << 48) - 1;
            uint64_t seed_;

        public:
            explicit JavaRandom(int64_t seed) { SetSeed(seed); }

            void SetSeed(int64_t seed) { seed_ = ((uint64_t)seed ^ MULTIPLIER) & MASK; }

            int32_t Next(int bits)
            {
                seed_ = (seed_ * MULTIPLIER + 0xBull) & MASK;
                return (int32_t)(uint32_t)(seed_ >> (48 - bits));
            }

            int32_t NextInt(int32_t bound)
            {
                if ((bound & -bound) == bound) // Power of 2
                    return (int32_t)(((int64_t)bound * (int64_t)Next(31)) >> 31);

                int32_t bits, value;
                do
                {
                    bits = Next(31);
                    value = bits % bound;
                } while ((int64_t)bits - value + (bound - 1) > INT32_MAX); // Overflow check from Java
                return value;
            }

            int64_t NextLong()
            {
                int64_t high = Next(32);
                int64_t low = Next(32);
                return (int64_t)(((uint64_t)high << 32) + (uint64_t)low);
            }

            float NextFloat() { return (float)Next(24) / (float)(1 << 24); }
    };

    // net.minecraft.util.MathHelper (lookup table based sin/cos)
    const std::array<float, 65536> &SinTable()
    {
        static const std::array<float, 65536> table = []() {
            std::array<float, 65536> values;
            for (int i = 0; i < 65536; i++)
                values[i] = (float)std::sin((double)i * 3.141592653589793 * 2.0 / 65536.0);
            return values;
        }();
        return table;
    }

    float McSin(float value) { return SinTable()[(int)(value * 10430.378f) & 65535]; }
    float McCos(float value) { return SinTable()[(int)(value * 10430.378f + 16384.0f) & 65535]; }

    int FloorDouble(double value)
    {
        int truncated = (int)value;
        return value < (double)truncated ? truncated - 1 : truncated;
    }

    // Carves the caves of every nearby source chunk into one 16x16 target section of a Lunacraft chunk
    class CaveCarver
    {
        private:
            BlockID *chunk_;
            int target_x_;   // Target section, in Minecraft chunk coordinates
            int target_z_;
            int offset_x_;   // Target section's offset inside the Lunacraft chunk
            int offset_z_;
            JavaRandom rand_{0};

            BlockID &BlockAt(int x, int y, int z) { return chunk_[GetChunkIndex(x + offset_x_, y, z + offset_z_)]; }

            static bool IsWater(BlockID block) { return block == BlockID::water; }
            static bool IsCarvable(BlockID block)
            {
                return block == BlockID::rock || block == BlockID::gravel || block == BlockID::dirt || block == BlockID::topsoil;
            }

            void GenerateLargeCaveNode(int64_t seed, double x, double y, double z)
            {
                float width = 1.0f + rand_.NextFloat() * 6.0f;
                GenerateCaveNode(seed, x, y, z, width, 0.0f, 0.0f, -1, -1, 0.5);
            }

            void GenerateCaveNode(int64_t seed, double x, double y, double z, float width, float yaw, float pitch, int step, int max_steps, double height_scale)
            {
                double center_x = (double)(target_x_ * MC_CHUNK_SIZE + 8);
                double center_z = (double)(target_z_ * MC_CHUNK_SIZE + 8);
                float yaw_velocity = 0.0f;
                float pitch_velocity = 0.0f;
                JavaRandom random{seed};

                if (max_steps <= 0)
                {
                    int max_length = RANGE * MC_CHUNK_SIZE - MC_CHUNK_SIZE;
                    max_steps = max_length - random.NextInt(max_length / 4);
                }

                bool is_room = false;
                if (step == -1)
                {
                    step = max_steps / 2;
                    is_room = true;
                }

                int split_step = random.NextInt(max_steps / 2) + max_steps / 4;
                bool is_steep = random.NextInt(6) == 0;

                for (; step < max_steps; step++)
                {
                    double horizontal_radius = 1.5 + (double)(McSin((float)step * PI / (float)max_steps) * width * 1.0f);
                    double vertical_radius = horizontal_radius * height_scale;

                    // Advance and drift the direction
                    float cos_pitch = McCos(pitch);
                    float sin_pitch = McSin(pitch);
                    x += (double)(McCos(yaw) * cos_pitch);
                    y += (double)sin_pitch;
                    z += (double)(McSin(yaw) * cos_pitch);

                    pitch *= is_steep ? 0.92f : 0.7f;
                    pitch += pitch_velocity * 0.1f;
                    yaw += yaw_velocity * 0.1f;
                    pitch_velocity *= 0.9f;
                    yaw_velocity *= 0.75f;

                    // Java evaluates operands left to right, so the random calls are sequenced explicitly
                    float a = random.NextFloat();
                    float b = random.NextFloat();
                    float c = random.NextFloat();
                    pitch_velocity += (a - b) * c * 2.0f;
                    a = random.NextFloat();
                    b = random.NextFloat();
                    c = random.NextFloat();
                    yaw_velocity += (a - b) * c * 4.0f;

                    // Split into two narrower tunnels heading off to either side
                    if (!is_room && step == split_step && width > 1.0f && max_steps > 0)
                    {
                        int64_t left_seed = random.NextLong();
                        float left_width = random.NextFloat() * 0.5f + 0.5f;
                        GenerateCaveNode(left_seed, x, y, z, left_width, yaw - PI / 2.0f, pitch / 3.0f, step, max_steps, 1.0);

                        int64_t right_seed = random.NextLong();
                        float right_width = random.NextFloat() * 0.5f + 0.5f;
                        GenerateCaveNode(right_seed, x, y, z, right_width, yaw + PI / 2.0f, pitch / 3.0f, step, max_steps, 1.0);
                        return;
                    }

                    if (!is_room && random.NextInt(4) == 0)
                        continue;

                    // Give up once the tunnel can no longer reach the target section
                    double dx = x - center_x;
                    double dz = z - center_z;
                    double steps_left = (double)(max_steps - step);
                    double max_reach = (double)(width + 2.0f + 16.0f);
                    if (dx * dx + dz * dz - steps_left * steps_left > max_reach * max_reach)
                        return;

                    if (x < center_x - 16.0 - horizontal_radius * 2.0 || z < center_z - 16.0 - horizontal_radius * 2.0
                     || x > center_x + 16.0 + horizontal_radius * 2.0 || z > center_z + 16.0 + horizontal_radius * 2.0)
                        continue;

                    int min_x = FloorDouble(x - horizontal_radius) - target_x_ * MC_CHUNK_SIZE - 1;
                    int max_x = FloorDouble(x + horizontal_radius) - target_x_ * MC_CHUNK_SIZE + 1;
                    int min_y = FloorDouble(y - vertical_radius) - 1;
                    int max_y = FloorDouble(y + vertical_radius) + 1;
                    int min_z = FloorDouble(z - horizontal_radius) - target_z_ * MC_CHUNK_SIZE - 1;
                    int max_z = FloorDouble(z + horizontal_radius) - target_z_ * MC_CHUNK_SIZE + 1;
                    if (min_x < 0) min_x = 0;
                    if (max_x > MC_CHUNK_SIZE) max_x = MC_CHUNK_SIZE;
                    if (min_y < 1) min_y = 1;
                    if (max_y > MAX_CARVE_Y) max_y = MAX_CARVE_Y;
                    if (min_z < 0) min_z = 0;
                    if (max_z > MC_CHUNK_SIZE) max_z = MC_CHUNK_SIZE;

                    // Don't carve next to water, so caves never drain or flood
                    bool found_water = false;
                    for (int bx = min_x; !found_water && bx < max_x; bx++)
                    {
                        for (int bz = min_z; !found_water && bz < max_z; bz++)
                        {
                            for (int by = max_y + 1; !found_water && by >= min_y - 1; by--)
                            {
                                if (by >= 0 && by < WORLD_HEIGHT_LIMIT)
                                {
                                    if (IsWater(BlockAt(bx, by, bz)))
                                        found_water = true;

                                    // Interior columns only need their top and bottom checked
                                    if (by != min_y - 1 && bx != min_x && bx != max_x - 1 && bz != min_z && bz != max_z - 1)
                                        by = min_y;
                                }
                            }
                        }
                    }
                    if (found_water)
                        continue;

                    for (int bx = min_x; bx < max_x; bx++)
                    {
                        double nx = ((double)(bx + target_x_ * MC_CHUNK_SIZE) + 0.5 - x) / horizontal_radius;
                        for (int bz = min_z; bz < max_z; bz++)
                        {
                            double nz = ((double)(bz + target_z_ * MC_CHUNK_SIZE) + 0.5 - z) / horizontal_radius;
                            if (nx * nx + nz * nz >= 1.0)
                                continue;

                            // Minecraft tests height `by` but edits the block one above it; kept for faithfulness
                            bool hit_topsoil = false;
                            for (int by = max_y - 1; by >= min_y; by--)
                            {
                                double ny = ((double)by + 0.5 - y) / vertical_radius;
                                if (ny <= -0.7 || nx * nx + ny * ny + nz * nz >= 1.0)
                                    continue;

                                int carve_y = by + 1;
                                BlockID &block = BlockAt(bx, carve_y, bz);
                                if (block == BlockID::topsoil)
                                    hit_topsoil = true;

                                if (IsCarvable(block))
                                {
                                    block = BlockID::air;

                                    // Regrow the surface when a cave opens up through it
                                    if (by >= LAVA_LEVEL && hit_topsoil && BlockAt(bx, carve_y - 1, bz) == BlockID::dirt)
                                        BlockAt(bx, carve_y - 1, bz) = BlockID::topsoil;
                                }
                            }
                        }
                    }

                    if (is_room)
                        break;
                }
            }

            // Rolls the caves that start in one source chunk
            void GenerateFromSourceChunk(int source_x, int source_z)
            {
                int cave_count = rand_.NextInt(rand_.NextInt(rand_.NextInt(40) + 1) + 1);
                if (rand_.NextInt(15) != 0)
                    cave_count = 0;

                for (int i = 0; i < cave_count; i++)
                {
                    double x = (double)(source_x * MC_CHUNK_SIZE + rand_.NextInt(16));
                    double y = (double)rand_.NextInt(rand_.NextInt(120) + 8);
                    double z = (double)(source_z * MC_CHUNK_SIZE + rand_.NextInt(16));

                    int tunnel_count = 1;
                    if (rand_.NextInt(4) == 0)
                    {
                        int64_t room_seed = rand_.NextLong();
                        GenerateLargeCaveNode(room_seed, x, y, z);
                        tunnel_count += rand_.NextInt(4);
                    }

                    for (int j = 0; j < tunnel_count; j++)
                    {
                        float yaw = rand_.NextFloat() * PI * 2.0f;
                        float pitch = (rand_.NextFloat() - 0.5f) * 2.0f / 8.0f;
                        float width = rand_.NextFloat() * 2.0f + rand_.NextFloat();
                        int64_t tunnel_seed = rand_.NextLong();
                        GenerateCaveNode(tunnel_seed, x, y, z, width, yaw, pitch, 0, 0, 1.0);
                    }
                }
            }

        public:
            CaveCarver(BlockID *chunk, int target_x, int target_z, int offset_x, int offset_z)
            : chunk_(chunk),
              target_x_(target_x),
              target_z_(target_z),
              offset_x_(offset_x),
              offset_z_(offset_z) {}

            void Carve(int64_t world_seed)
            {
                rand_.SetSeed(world_seed);
                int64_t x_multiplier = rand_.NextLong() / 2 * 2 + 1;
                int64_t z_multiplier = rand_.NextLong() / 2 * 2 + 1;

                for (int source_x = target_x_ - RANGE; source_x <= target_x_ + RANGE; source_x++)
                {
                    for (int source_z = target_z_ - RANGE; source_z <= target_z_ + RANGE; source_z++)
                    {
                        // Java long arithmetic wraps, so this is done unsigned
                        uint64_t source_seed = (uint64_t)(int64_t)source_x * (uint64_t)x_multiplier
                                             + (uint64_t)(int64_t)source_z * (uint64_t)z_multiplier;
                        rand_.SetSeed((int64_t)(source_seed ^ (uint64_t)world_seed));
                        GenerateFromSourceChunk(source_x, source_z);
                    }
                }
            }
    };
}

void CarveCaves(BlockID *chunk, int chunk_x, int chunk_z, uint64_t seed)
{
    constexpr int SECTIONS = CHUNK_SIZE / MC_CHUNK_SIZE;
    for (int section_x = 0; section_x < SECTIONS; section_x++)
    {
        for (int section_z = 0; section_z < SECTIONS; section_z++)
        {
            CaveCarver carver{
                chunk,
                chunk_x * SECTIONS + section_x,
                chunk_z * SECTIONS + section_z,
                section_x * MC_CHUNK_SIZE,
                section_z * MC_CHUNK_SIZE
            };
            carver.Carve((int64_t)seed);
        }
    }
}
