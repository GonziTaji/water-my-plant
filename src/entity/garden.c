
#include "garden.h"
#include "plant.h"
#include <raylib.h>
#include <stdio.h>

int totalPlanters() {
    return GARDEN_COLS * GARDEN_ROWS;
}

void garden_init(Garden *garden) {
    garden->selectedPlanter = 0;

    for (int i = 0; i < totalPlanters(); i++) {
        int col = i == 0 ? 0 : i % GARDEN_COLS;
        int row = i == 0 ? 0 : i / GARDEN_COLS;
        float x = GARDEN_ORIGIN_X + col * (PLANTER_WIDTH + SPACING_X);
        float y = GARDEN_ORIGIN_Y + row * (PLANTER_HEIGHT + SPACING_Y);

        printf("i %d x %f y %f w %d h %d\n", i, x, y, PLANTER_WIDTH, PLANTER_HEIGHT);
        planter_init(&garden->planters[i], (Rectangle){x, y, PLANTER_WIDTH, PLANTER_HEIGHT});
    }
}

void garden_update(Garden *garden, float deltaTime) {
    for (int i = 0; i < totalPlanters(); i++) {
        if (garden->planters[i].hasPlant) {
            plant_update(&garden->planters[i].plant, deltaTime);
        }
    }
}

void garden_draw(Garden *garden) {
    for (int i = 0; i < totalPlanters(); i++) {
        Color planterBorderColor = garden->selectedPlanter == i ? BEIGE : BROWN;
        Rectangle bounds = garden->planters[i].bounds;

        DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, planterBorderColor);

        if (garden->planters[i].hasPlant) {
            plant_draw(&garden->planters[i].plant, (Vector2){bounds.x, bounds.y});
        }
    }
}

int getPlanterIndexFromCoords(int col, int row) {
    return row * GARDEN_COLS + col;
}

int garden_getPlanterIndexFromPoint(Vector2 point) {
    if (point.x < GARDEN_ORIGIN_X || point.y < GARDEN_ORIGIN_Y) {
        return -1;
    }

    int col = (point.x - GARDEN_ORIGIN_X) / (PLANTER_WIDTH + SPACING_X);
    int row = (point.y - GARDEN_ORIGIN_Y) / (PLANTER_HEIGHT + SPACING_Y);

    printf("px %f py %f c %d r %d\n", point.x, point.y, col, row);

    if (row >= 0 && row < GARDEN_ROWS && col >= 0 && col < GARDEN_COLS) {
        return getPlanterIndexFromCoords(col, row);
    } else {
        return -1;
    }
}
