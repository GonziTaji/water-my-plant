
#include "garden.h"
#include "../game/constants.h"
#include "plant.h"
#include <raylib.h>

Texture2D gardenTexture;

typedef struct {
    Vector2 vertices[4];
} RectVertices;

Vector2 gridCoordsToWorldCoords(float x, float y) {
    return (Vector2){
        (x - y) * PLANTER_WIDTH * WORLD_SCALE / 2.0f,
        (x + y) * PLANTER_HEIGHT * WORLD_SCALE / 2.0f,
    };
}

Vector2 screenCoordsToGridCoords(float x, float y) {
    float gardenX = (x - GARDEN_ORIGIN_X) / WORLD_SCALE;
    float gardenY = (y - GARDEN_ORIGIN_Y) / WORLD_SCALE;

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
    planter_loadTextures();

    garden->selectedPlanter = 0;
    garden->plantersCount = GARDEN_COLS * GARDEN_ROWS;

    gardenTexture = LoadTexture("resources/assets/floor.png");
    SetTextureFilter(gardenTexture, TEXTURE_FILTER_BILINEAR);

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

    garden->hoveredPlanter = cellHoveredIndex;

    if (input->mouseButtonPressed[0]) {
        garden->selectedPlanter = cellHoveredIndex;
    }
}

void garden_draw(Garden *garden) {
    Rectangle source = {0, 0, gardenTexture.width, gardenTexture.height};

    Rectangle dest = {
        GARDEN_ORIGIN_X,
        GARDEN_ORIGIN_Y,
        gardenTexture.width * WORLD_SCALE,
        gardenTexture.height * WORLD_SCALE,
    };

    Vector2 origin = {dest.width / 2, 0};

    DrawTexturePro(gardenTexture, source, dest, origin, 0, WHITE);

    for (int i = 0; i < garden->plantersCount; i++) {
        Vector2 *rv = getPlanterIsoVertices(i).vertices;

        if (garden->selectedPlanter == i || garden->hoveredPlanter == i) {
            Color planterBorderColor = garden->selectedPlanter == i ? DARKBROWN : BROWN;

            DrawLineEx(rv[0], rv[1], 2, planterBorderColor);
            DrawLineEx(rv[1], rv[2], 2, planterBorderColor);
            DrawLineEx(rv[2], rv[3], 2, planterBorderColor);
            DrawLineEx(rv[3], rv[0], 2, planterBorderColor);
        }

        Vector2 tileBase = {rv[2].x, rv[2].y};

        planter_draw(&garden->planters[i], tileBase);

        Vector2 plantOrigin = {tileBase.x, tileBase.y - garden->planters[i].height};

        if (garden->planters[i].hasPlant) {
            plant_draw(&garden->planters[i].plant, plantOrigin);
        }
    }
}

void garden_unload() {
    UnloadTexture(gardenTexture);
    plant_unloadTextures();
    planter_unloadTextures();
}
