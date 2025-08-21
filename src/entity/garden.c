
#include "garden.h"
#include "plant.h"
#include <math.h>
#include <raylib.h>

Texture2D texture;

double degToRad(float degrees) {
    return degrees * (M_PI / 180);
}

typedef struct {
    Vector2 vertices[4];
} RectVertices;

bool pointInsideVertices(Vector2 point, RectVertices rv) {
    int i, j;

    bool inside = false;

    for (i = 0, j = 3; i < 4; j = i++) {
        Vector2 vi = rv.vertices[i];
        Vector2 vj = rv.vertices[j];
        if (((vi.y > point.y) != (vj.y > point.y)) &&
            (point.x < (vj.x - vi.x) * (point.y - vi.y) / (vj.y - vi.y) + vi.x)) {
            inside = !inside;
        }
    }
    return inside;
}

Vector2 gridCoordsToWorldCoords(float x, float y) {
    return (Vector2){
        (x - y) * PLANTER_WIDTH / 2.0f,
        (x + y) * PLANTER_HEIGHT / 2.0f,
    };
}

Vector2 screenCoordsToGridCoords(float x, float y) {
    float gardenX = x - GARDEN_ORIGIN_X;
    float gardenY = y - GARDEN_ORIGIN_Y;

    float gx = (gardenX / (PLANTER_WIDTH / 2.0f) + gardenY / (PLANTER_HEIGHT / 2.0f)) / 2.0f;
    float gy = (gardenY / (PLANTER_HEIGHT / 2.0f) - gardenX / (PLANTER_WIDTH / 2.0f)) / 2.0f;
    return (Vector2){gx, gy};
}

bool isValidGridCoords(int x, int y) {
    if (x < 0 || y < 0 || x >= GARDEN_COLS || y >= GARDEN_ROWS) {
        return false;
    }

    return true;
}

Vector2 getPlanterCoordsFromIndex(int i) {
    return (Vector2){
        i % GARDEN_COLS,
        (int)(i / GARDEN_COLS),
    };
}

/** Returns -1 if the coords are not a valid cell of the grid */
int getPlanterIndexFromCoords(int x, int y) {
    if (!isValidGridCoords(x, y)) {
        return -1;
    }

    return (y * GARDEN_COLS) + x;
}

RectVertices getPlanterIsoVertices(int planterIndex) {
    Vector2 coords = getPlanterCoordsFromIndex(planterIndex);

    RectVertices rv = {
        .vertices =
            {
                gridCoordsToWorldCoords(coords.x, coords.y),
                gridCoordsToWorldCoords(coords.x + 1, coords.y),
                gridCoordsToWorldCoords(coords.x + 1, coords.y + 1),
                gridCoordsToWorldCoords(coords.x, coords.y + 1),
            },
    };

    for (int i = 0; i < 4; i++) {
        rv.vertices[i].x += GARDEN_ORIGIN_X;
        rv.vertices[i].y += GARDEN_ORIGIN_Y;
    }

    return rv;
}

void garden_init(Garden *garden) {
    plant_loadTextures();

    garden->selectedPlanter = 0;
    garden->plantersCount = GARDEN_COLS * GARDEN_ROWS;

    texture = LoadTexture("resources/assets/balcony_floor.png");

    for (int i = 0; i < garden->plantersCount; i++) {
        planter_init(&garden->planters[i]);
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
    Vector2 cellHoveredCoords =
        screenCoordsToGridCoords(input->worldMousePos.x, input->worldMousePos.y);

    int cellHoveredIndex = getPlanterIndexFromCoords(cellHoveredCoords.x, cellHoveredCoords.y);

    // if (cellHoveredIndex != -1) {
    garden->hoveredPlanter = cellHoveredIndex;

    if (input->mouseButtonPressed[0]) {
        garden->selectedPlanter = cellHoveredIndex;
    }
    // }
}

void garden_draw(Garden *garden) {
    DrawTexture(texture, 300, 130, WHITE);

    for (int i = 0; i < garden->plantersCount; i++) {
        Vector2 *rv = getPlanterIsoVertices(i).vertices;

        if (garden->planters[i].hasPlant) {
            plant_draw(&garden->planters[i].plant, (Vector2){rv[2].x, rv[2].y});
        }

        if (garden->selectedPlanter == i || garden->hoveredPlanter == i) {
            Color planterBorderColor = garden->selectedPlanter == i ? BEIGE : BROWN;

            DrawLineEx(rv[0], rv[1], 2, planterBorderColor);
            DrawLineEx(rv[1], rv[2], 2, planterBorderColor);
            DrawLineEx(rv[2], rv[3], 2, planterBorderColor);
            DrawLineEx(rv[3], rv[0], 2, planterBorderColor);
        }
    }
}

void garden_unload() {
    UnloadTexture(texture);
    plant_unloadTextures();
}
