#pragma once

#include "../game/constants.h"
#include "../game/gameplay.h"
#include "../input/input.h"
#include "../messages/messages.h"
#include "../utils/utils.h"
#include "planter.h"
#include <raylib.h>

#define GARDEN_MAX_COLS 20
#define GARDEN_MAX_ROWS 20
#define GARDEN_MAX_TILES (GARDEN_MAX_COLS * GARDEN_MAX_ROWS)
#define GARDEN_SCALE_INITIAL (1.0f * WORLD_SCALE)
#define GARDEN_SCALE_STEP 0.4f
#define GARDEN_SCALE_MIN GARDEN_SCALE_INITIAL - (2 * GARDEN_SCALE_STEP)
#define GARDEN_SCALE_MAX GARDEN_SCALE_INITIAL + (5 * GARDEN_SCALE_STEP)

// TODO: maybe export to it's own file
// Maybe don't use this lol
typedef struct {
    int planterIndex;
    int lightLevel;
} GardenTile;

typedef struct {
    SceneTransform transform;
    Vector2 *screenSize;
    TileGrid tileGrid;
    GardenTile tiles[GARDEN_MAX_TILES];
    int tileSelected;
    int tileHovered;
    int planterTileHovered;
    Planter planters[GARDEN_MAX_TILES];
    Vector2 lightSourcePos;
    int lightSourceLevel;
    Rotation selectionRotation;
} Garden;

void garden_init(Garden *garden, Vector2 *screenSize, float gameplayTime);
Message garden_processInput(Garden *garden, InputManager *input);
void garden_draw(Garden *garden, enum GardeningTool toolSelected, int toolVariantSelected);
void garden_update(Garden *garden, float deltaTime, float gameplayTime);
bool garden_hasPlanterSelected(const Garden *garden);
Planter *garden_getSelectedPlanter(Garden *garden);
Vector2 garden_getTileOrigin(Garden *garden, Vector2 coords);
