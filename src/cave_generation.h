#pragma once

#include <cstdint>

#include "block.h"

// Carves Minecraft-style (pre-1.7) worm caves into a freshly generated chunk
void CarveCaves(BlockID *chunk, int chunk_x, int chunk_z, uint64_t seed);
