#pragma once

#include "../commands/commands.h"
#include "../ui/input_manager.h"
#include "planter.h"
#include <raylib.h>

#define GARDEN_MAX_TILES 100

// TODO: maybe export to it's own file
typedef struct {
    bool hasPlanter;
    Planter planter;
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
    Vector2 lightSourcePos;
    int lightSourceLevel;
} Garden;

void garden_init(Garden *garden, Vector2 screenSize, float gameplayTime);
Command garden_processInput(Garden *garden, InputManager *input);
void garden_draw(Garden *garden);
void garden_update(Garden *garden, float deltaTime, float gameplayTime);
bool garden_hasPlanterSelected(const Garden *garden);
Planter *garden_getSelectedPlanter(Garden *garden);
