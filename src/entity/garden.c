
#include "garden.h"
#include "plant.h"
#include <raylib.h>

void garden_init(Garden *garden) {
    garden->selectedPlanter = 0;
    garden->plantersCount = GARDEN_COLS * GARDEN_ROWS;

    for (int i = 0; i < garden->plantersCount; i++) {
        int col = i == 0 ? 0 : i % GARDEN_COLS;
        int row = i == 0 ? 0 : i / GARDEN_COLS;
        float x = GARDEN_ORIGIN_X + col * (PLANTER_WIDTH + PLANTER_SPACING_X);
        float y = GARDEN_ORIGIN_Y + row * (PLANTER_HEIGHT + PLANTER_SPACING_Y);

        planter_init(&garden->planters[i], (Rectangle){x, y, PLANTER_WIDTH, PLANTER_HEIGHT});
    }
}

void garden_update(Garden *garden, float deltaTime) {
    for (int i = 0; i < garden->plantersCount; i++) {
        if (garden->planters[i].hasPlant) {
            plant_update(&garden->planters[i].plant, deltaTime);
        }
    }
}

void garden_processClick(Garden *garden, const InputManager *input) {
    if (input->mouseButtonPressed[0]) {
        int clickedPlanterIndex = garden_getPlanterIndexFromPoint(input->mousePosVirtual);

        if (clickedPlanterIndex != -1) {
            garden->selectedPlanter = clickedPlanterIndex;
        }
    }
}

void garden_draw(Garden *garden) {
    for (int i = 0; i < garden->plantersCount; i++) {
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

    int col = (point.x - GARDEN_ORIGIN_X) / (PLANTER_WIDTH + PLANTER_SPACING_X);
    int row = (point.y - GARDEN_ORIGIN_Y) / (PLANTER_HEIGHT + PLANTER_SPACING_Y);

    if (row >= 0 && row < GARDEN_ROWS && col >= 0 && col < GARDEN_COLS) {
        return getPlanterIndexFromCoords(col, row);
    } else {
        return -1;
    }
}
