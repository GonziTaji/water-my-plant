
#pragma once

#define SECONDS_IN_A_DAY 86400            // seconds in a 24h day - 24*60*60
#define GAME_SECONDS_PER_RL_SECONDS 20.0f // game seconds equivalent to a real second

enum GameplayMode { GAMEPLAY_MODE_NORMAL, GAMEPLAY_MODE_IRRIGATION };

typedef enum {
    GAMEPLAY_SPEED_NORMAL,
    GAMEPLAY_SPEED_FAST,
    GAMEPLAY_SPEED_FASTEST,
    GAMEPLAY_SPEED_COUNT,
} GameplaySpeed;

typedef enum {
    ROTATION_0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270,
    ROTATION_COUNT,
} Rotation;

// Move to somewhere else?
enum GardeningTool {
    GARDENING_TOOL_NONE,
    GARDENING_TOOL_IRRIGATOR,
    GARDENING_TOOL_NUTRIENTS,
    GARDENING_TOOL_PLANTER,
    GARDENING_TOOL_PLANT_CUTTING,
    GARDENING_TOOL_TRASH_BIN,
    GARDENING_TOOL_COUNT,
};
