#pragma once

#include "planter.h"

#define GARDEN_ROWS 1
#define GARDEN_COLS 6
#define PLANTER_WIDTH 100
#define PLANTER_HEIGHT 100
#define PLANTER_SPACING_X 8
#define PLANTER_SPACING_Y 8
#define GARDEN_ORIGIN_X 10
#define GARDEN_ORIGIN_Y 100

typedef struct {
    Planter planters[GARDEN_ROWS * GARDEN_COLS];
    int selectedPlanter;
    int plantersCount;
} Garden;

void garden_init(Garden *garden);
int garden_getPlanterIndexFromPoint(Vector2 point);
void garden_draw(Garden *garden);
void garden_update(Garden *garden, float deltaTime);
