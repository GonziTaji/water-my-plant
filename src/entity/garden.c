
#include "garden.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "plant.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>

#define LIGHT_SOURCE_RADIUS 40
#define MAX_TIME 1440         // minutes in a 24h day
#define SECONDS_PER_MINUTE 10 // real seconds equivalent to a game minute

typedef struct {
    Vector2 vertices[4];
} RectVertices;

/// `timeOfDay` is the % of the day passed (time/MAX_TIME)
float getLightSourceHeight(float timeOfDay) {
    // 4x^2 - x
    // because:
    // x = 0 (sunrise) => y = 0
    // x = 1 (sunset) => y = 0
    // x = 0.5 (noon) => y = 0.5
    return 4.0f * timeOfDay * -(1.0f - timeOfDay);
}

// Linear interpolation
float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

Vector2 getLightSourcePosition(float time) {
    float timeOfDay = time / MAX_TIME;
    int maxHeight = 256;

    // position in a 1x1 universe
    Vector2 v = {
        lerp(0, 1, timeOfDay),
        getLightSourceHeight(timeOfDay),
    };

    // scale and translate
    v.x *= (GARDEN_COLS)*PLANTER_WIDTH;
    v.y *= maxHeight;

    float rad = 25 * (M_PI / 180);

    Vector2 rotated = {
        v.x * cos(rad) - v.y * sin(rad),
        v.y * cos(rad) + v.x * sin(rad),
    };

    rotated.x += GARDEN_ORIGIN_X + PLANTER_WIDTH;
    rotated.y += GARDEN_ORIGIN_Y - PLANTER_HEIGHT;

    return rotated;
}

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
    garden->time = 0;
    garden->secondsPassed = 0;
    garden->lightSourceLevel = 12;
    garden->lightSourcePos = getLightSourcePosition(garden->time);
    garden->tileSelected = 0;
    garden->tilesCount = GARDEN_COLS * GARDEN_ROWS;

    for (int i = 0; i < garden->tilesCount; i++) {
        garden->tiles[i].hasPlanter = false;
    }

    updateLightLevelOfTiles(garden);
}

void garden_update(Garden *garden, float deltaTime) {
    garden->secondsPassed += deltaTime;

    if (garden->secondsPassed >= SECONDS_PER_MINUTE) {
        garden->time += 1;
        garden->secondsPassed = 0;

        if (garden->time > MAX_TIME) {
            garden->time = 0;
        }

        garden->lightSourcePos = getLightSourcePosition(garden->time);
        updateLightLevelOfTiles(garden);
    }

    for (int i = 0; i < garden->tilesCount; i++) {
        if (garden->tiles[i].hasPlanter && garden->tiles[i].planter.hasPlant) {
            plant_update(&garden->tiles[i].planter.plant, deltaTime);
        }
    }

    // to test time
    if (IsKeyDown(KEY_T)) {
        garden->time += 5;
        garden->lightSourcePos = getLightSourcePosition(garden->time);
        updateLightLevelOfTiles(garden);
        if (garden->time > MAX_TIME) {
            garden->time = 0;
        }
    }
}

void garden_processClick(Garden *garden, InputManager *input) {
    Vector2 cellHoveredCoords =
        screenCoordsToGridCoords(input->worldMousePos.x, input->worldMousePos.y);

    int cellHoveredIndex = getPlanterIndexFromCoords(cellHoveredCoords.x, cellHoveredCoords.y);

    garden->tileHovered = cellHoveredIndex;

    if (input->mouseButtonPressed[MOUSE_BUTTON_LEFT]) {
        garden->tileSelected = cellHoveredIndex;
        input->mouseButtonPressed[MOUSE_BUTTON_LEFT] = false;
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

    char buffer[64];
    int hours = garden->time / 60;
    int minutes = garden->time % 60;
    snprintf(buffer,
        64,
        "Time: %02d:%02d:%02d",
        hours,
        minutes,
        (int)(garden->secondsPassed * 60 / SECONDS_PER_MINUTE));
    DrawText(buffer, 300, 0, 30, WHITE);

    snprintf(buffer, 64, "Raw time: %d", garden->time);
    DrawText(buffer, 300, 30, 30, WHITE);
}
