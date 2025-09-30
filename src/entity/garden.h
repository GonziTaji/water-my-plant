#pragma once

#include "../game/constants.h"
#include "../game/gameplay.h"
#include "../game/scenes/scene.h"
#include "../input/input.h"
#include "../messages/messages.h"
#include "planter.h"
#include <raylib.h>

#define GARDEN_MAX_COLS 20
#define GARDEN_MAX_ROWS 20
#define GARDEN_MAX_TILES (GARDEN_MAX_COLS * GARDEN_MAX_ROWS)

// TODO: maybe export to it's own file
// Maybe don't use this lol
typedef struct {
    int planterIndex;
    int lightLevel;
} GardenTile;

typedef struct {
    GardenTile tiles[GARDEN_MAX_TILES];
    int tileSelected;
    int tileHovered;
    int planterPickedUpIndex;
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
void garden_updateGardenOrigin(Garden *garden, Vector2 *screenSize);
