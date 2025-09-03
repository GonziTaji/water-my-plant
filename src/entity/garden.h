#pragma once

#include "../commands/commands.h"
#include "../ui/input_manager.h"
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
    Vector2 origin;
    int tileCols;
    int tileRows;
    GardenTile tiles[GARDEN_MAX_TILES];
    int tileWidth;
    int tileHeight;
    int tileSelected;
    int tileHovered;
    int tilesCount;
    Planter planters[GARDEN_MAX_TILES];
    Vector2 lightSourcePos;
    int lightSourceLevel;
} Garden;

void garden_init(Garden *garden, Vector2 screenSize, float gameplayTime);
Command garden_processInput(Garden *garden, InputManager *input);
void garden_draw(Garden *garden);
void garden_update(Garden *garden, float deltaTime, float gameplayTime);
bool garden_hasPlanterSelected(const Garden *garden);
Planter *garden_getSelectedPlanter(Garden *garden);
Vector2 garden_getPlanterCoordsFromIndex(Garden *garden, int i);
int garden_getPlanterIndexFromCoords(Garden *garden, int x, int y);
