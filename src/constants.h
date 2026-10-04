#pragma once

constexpr int CHUNK_SIZE = 32;
constexpr int WORLD_HEIGHT_LIMIT = 128;
constexpr int BLOCKS_IN_CHUNK = CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT_LIMIT;
constexpr int GROUND_LEVEL = 64;
constexpr int LIGHT_PHASES = 12;
// Nights run at 2x speed (see Moon::Update), so a period of 24/19 * 20 min gives a real-time
// cycle of 20 minutes like Minecraft's: ~12.6 min of daylight and ~7.4 min of darkness
constexpr float LIGHT_CYCLE_PERIOD = 20.0f * 60.0f * 24.0f / 19.0f;
constexpr float LIGHT_CYCLE_OMEGA = 2.0f * 3.14159f / LIGHT_CYCLE_PERIOD;
constexpr float FIXED_DELTA_TIME = 1.0f / 60.0f; // 60 Hz
constexpr int MAX_RENDER_DISTANCE = 24;
constexpr float PLAYER_GRAVITY = 5.0f;
constexpr float MOB_GRAVITY = 2.0f * PLAYER_GRAVITY;
