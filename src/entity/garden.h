#pragma once

#include "../ui/input_manager.h"
#include "planter.h"

#define GARDEN_ROWS 2
#define GARDEN_COLS 6
#define PLANTER_WIDTH 80
#define PLANTER_HEIGHT 80
#define PLANTER_SPACING_X 8
#define PLANTER_SPACING_Y 8
#define GARDEN_ORIGIN_X 900
#define GARDEN_ORIGIN_Y 160

typedef struct {
    Planter planters[GARDEN_ROWS * GARDEN_COLS];
    int selectedPlanter;
    int plantersCount;
} Garden;

void garden_init(Garden *garden);
void garden_processClick(Garden *garden, const InputManager *input);
void garden_draw(Garden *garden);
void garden_update(Garden *garden, float deltaTime);
void garden_unload();
