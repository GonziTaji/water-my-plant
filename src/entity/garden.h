#pragma once

#include "../commands/commands.h"
#include "../ui/input_manager.h"
#include "planter.h"

#define GARDEN_ROWS 6
#define GARDEN_COLS 6
#define PLANTER_WIDTH 128
#define PLANTER_HEIGHT 64
#define GARDEN_ORIGIN_X 900
#define GARDEN_ORIGIN_Y 160

// TODO: maybe export to it's own file
typedef struct {
    bool hasPlanter;
    Planter planter;
    int lightLevel;
} GardenTile;

typedef struct {
    GardenTile tiles[GARDEN_ROWS * GARDEN_COLS];
    int tileSelected;
    int tileHovered;
    int tilesCount;
    Vector2 lightSourcePos;
    int lightSourceLevel;
    int time;
    float secondsPassed;
} Garden;

void garden_init(Garden *garden);
Command garden_processInput(Garden *garden, InputManager *input);
void garden_draw(Garden *garden);
void garden_update(Garden *garden, float deltaTime);
bool garden_hasPlanterSelected(const Garden *garden);
Planter *garden_getSelectedPlanter(Garden *garden);
