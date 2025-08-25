
#include "garden.h"
#include "../game/constants.h"
#include "plant.h"
#include <math.h>
#include <raylib.h>

#define LIGHT_SOURCE_RADIUS 40

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

void updateLightLevelOfTiles(Garden *garden) {
    Vector2 lightSourceInGrid =
        screenCoordsToGridCoords(garden->lightSourcePos.x, garden->lightSourcePos.y);

    for (int i = 0; i < garden->tilesCount; i++) {
        Vector2 tileCoords = getPlanterCoordsFromIndex(i);
        float distance = sqrtf(pow(tileCoords.x - lightSourceInGrid.x, 2) +
                               pow(tileCoords.y - lightSourceInGrid.y, 2));
        garden->tiles[i].lightLevel = garden->lightSourceLevel - distance;
    }
}

void garden_init(Garden *garden) {
    plant_loadTextures();
    planter_loadTextures();

    garden->lightSourceLevel = 12;
    garden->lightSourcePos = (Vector2){GARDEN_ORIGIN_X + 100, GARDEN_ORIGIN_Y - 100};
    garden->tileSelected = 0;
    garden->tilesCount = GARDEN_COLS * GARDEN_ROWS;

    gardenTexture = LoadTexture("resources/assets/floor.png");
    SetTextureFilter(gardenTexture, TEXTURE_FILTER_BILINEAR);

    for (int i = 0; i < garden->tilesCount; i++) {
        garden->tiles[i].hasPlanter = false;
    }

    updateLightLevelOfTiles(garden);
}

void garden_update(Garden *garden, float deltaTime) {
    for (int i = 0; i < garden->tilesCount; i++) {

        if (garden->tiles[i].hasPlanter && garden->tiles[i].planter.hasPlant) {
            plant_update(&garden->tiles[i].planter.plant, deltaTime);
        }
    }
}

void garden_processClick(Garden *garden, InputManager *input) {
    // Drag light source to test mechanic
    bool isMouseInLightSource = CheckCollisionPointCircle(
        input->worldMousePos, garden->lightSourcePos, LIGHT_SOURCE_RADIUS);

    if (isMouseInLightSource && IsMouseButtonDown(0)) {
        garden->lightSourcePos = (Vector2){
            garden->lightSourcePos.x - input->worldMouseDelta.x,
            garden->lightSourcePos.y - input->worldMouseDelta.y,
        };

        updateLightLevelOfTiles(garden);

        input->mouseButtonPressed[0] = false;
    }

    Vector2 cellHoveredCoords =
        screenCoordsToGridCoords(input->worldMousePos.x, input->worldMousePos.y);

    int cellHoveredIndex = getPlanterIndexFromCoords(cellHoveredCoords.x, cellHoveredCoords.y);

    garden->tileHovered = cellHoveredIndex;

    if (input->mouseButtonPressed[0]) {
        garden->tileSelected = cellHoveredIndex;
        input->mouseButtonPressed[0] = false;
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

    for (int i = 0; i < garden->tilesCount; i++) {
        Vector2 *rv = getPlanterIsoVertices(i).vertices;

        if (garden->tileSelected == i || garden->tileHovered == i) {
            Color planterBorderColor = garden->tileSelected == i ? DARKBROWN : BROWN;

            DrawLineEx(rv[0], rv[1], 2, planterBorderColor);
            DrawLineEx(rv[1], rv[2], 2, planterBorderColor);
            DrawLineEx(rv[2], rv[3], 2, planterBorderColor);
            DrawLineEx(rv[3], rv[0], 2, planterBorderColor);
        }

        Vector2 tileBase = {rv[2].x, rv[2].y};

        if (garden->tiles[i].hasPlanter) {
            Planter *planter = &garden->tiles[i].planter;

            planter_draw(&garden->tiles[i].planter, tileBase);

            if (planter->hasPlant) {
                Vector2 plantOrigin = {tileBase.x, tileBase.y - (planter->height * WORLD_SCALE)};
                plant_draw(&planter->plant, plantOrigin);
            }
        }
    }

    DrawCircle(garden->lightSourcePos.x, garden->lightSourcePos.y, LIGHT_SOURCE_RADIUS, YELLOW);
}

void garden_unload() {
    UnloadTexture(gardenTexture);
    plant_unloadTextures();
    planter_unloadTextures();
}
