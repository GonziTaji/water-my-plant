
#include "garden.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "plant.h"
#include <math.h>
#include <raylib.h>

#define LIGHT_SOURCE_RADIUS 40
#define TILE_WIDTH 128
#define TILE_HEIGHT (int)(TILE_WIDTH / 2)

typedef struct {
    Vector2 vertices[4];
} RectVertices;

typedef struct {
    Vector2 top;
    Vector2 right;
    Vector2 bottom;
    Vector2 left;
} IsoRec;

/// Linear interpolation
float lerp(float start, float stop, float amount) {
    return start + (stop - start) * amount;
}

/// `timeOfDay` is the % of the day passed
float getLightSourceHeight(float timeOfDay) {
    // 4x^2 - x
    // because:
    // x = 0 (sunrise) => y = 0
    // x = 1 (sunset) => y = 0
    // x = 0.5 (noon) => y = 0.5
    return 4.0f * timeOfDay * -(1.0f - timeOfDay);
}

Vector2 getLightSourcePosition(Garden *garden, int gameplayTime) {
    float timeOfDay = (float)gameplayTime / (float)SECONDS_IN_A_DAY;
    int maxHeight = 256;

    // position in a 1x1 universe
    Vector2 v = {
        lerp(0, 1, timeOfDay),
        getLightSourceHeight(timeOfDay),
    };

    // scale and translate
    v.x *= (garden->tileCols) * TILE_WIDTH;
    v.y *= maxHeight;

    float rad = 25 * (M_PI / 180);

    Vector2 rotated = {
        v.x * cos(rad) - v.y * sin(rad),
        v.y * cos(rad) + v.x * sin(rad),
    };

    rotated.x += garden->origin.x + TILE_WIDTH;
    rotated.y += garden->origin.y - TILE_HEIGHT;

    return rotated;
}

Vector2 gridCoordsToWorldCoords(Garden *garden, float x, float y) {
    return (Vector2){
        garden->origin.x + ((x - y) * TILE_WIDTH * WORLD_SCALE / 2.0f),
        garden->origin.y + ((x + y) * TILE_HEIGHT * WORLD_SCALE / 2.0f),
    };
}

Vector2 screenCoordsToGridCoords(Garden *garden, float x, float y) {
    float gardenX = (x - garden->origin.x) / WORLD_SCALE;
    float gardenY = (y - garden->origin.y) / WORLD_SCALE;

    return (Vector2){
        (gardenX / (TILE_WIDTH / 2.0f) + gardenY / (TILE_HEIGHT / 2.0f)) / 2.0f,
        (gardenY / (TILE_HEIGHT / 2.0f) - gardenX / (TILE_WIDTH / 2.0f)) / 2.0f,
    };
}

bool isValidGridCoords(Garden *garden, int x, int y) {
    if (x < 0 || y < 0 || x >= garden->tileCols || y >= garden->tileCols) {
        return false;
    }

    return true;
}

Vector2 getPlanterCoordsFromIndex(Garden *garden, int i) {
    return (Vector2){
        i % garden->tileCols,
        (int)(i / garden->tileCols),
    };
}

/** Returns -1 if the coords are not a valid cell of the grid */
int getPlanterIndexFromCoords(Garden *garden, int x, int y) {
    if (!isValidGridCoords(garden, x, y)) {
        return -1;
    }

    return (y * garden->tileCols) + x;
}

IsoRec getGardenIsoVertices(Garden *garden) {
    IsoRec isoRec = {
        gridCoordsToWorldCoords(garden, 0, 0),
        gridCoordsToWorldCoords(garden, garden->tileCols, 0),
        gridCoordsToWorldCoords(garden, garden->tileCols, garden->tileRows),
        gridCoordsToWorldCoords(garden, 0, garden->tileRows),
    };

    return isoRec;
}

IsoRec getPlanterIsoVertices(Garden *garden, int planterIndex) {
    Vector2 coords = getPlanterCoordsFromIndex(garden, planterIndex);

    IsoRec isoRec = {
        gridCoordsToWorldCoords(garden, coords.x, coords.y),
        gridCoordsToWorldCoords(garden, coords.x + 1, coords.y),
        gridCoordsToWorldCoords(garden, coords.x + 1, coords.y + 1),
        gridCoordsToWorldCoords(garden, coords.x, coords.y + 1),
    };

    return isoRec;
}

float distanceBetweenPoints(Vector2 p1, Vector2 p2) {
    return sqrtf(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void updateLightLevelOfTiles(Garden *garden) {
    Vector2 lightSourceInGrid =
        screenCoordsToGridCoords(garden, garden->lightSourcePos.x, garden->lightSourcePos.y);

    for (int i = 0; i < garden->tilesCount; i++) {
        Vector2 tileCoords = getPlanterCoordsFromIndex(garden, i);

        float distance = distanceBetweenPoints(tileCoords, lightSourceInGrid);
        garden->tiles[i].lightLevel = garden->lightSourceLevel - distance;
    }
}

void garden_init(Garden *garden, Vector2 screenSize, float gameplayTime) {
    garden->lightSourceLevel = 12;
    garden->tileSelected = 0;
    garden->tileHovered = 0;
    garden->tileCols = 10;
    garden->tileRows = 10;
    garden->tilesCount = garden->tileCols * garden->tileRows;
    garden->origin.x = screenSize.x / 2;
    garden->origin.y = 200;

    for (int i = 0; i < garden->tilesCount; i++) {
        garden->tiles[i].hasPlanter = false;
        garden->tiles[i].planter.hasPlant = false;
    }

    garden->lightSourcePos = getLightSourcePosition(garden, gameplayTime);
    updateLightLevelOfTiles(garden);
}

bool garden_hasPlanterSelected(const Garden *garden) {
    return garden->tileSelected != -1 && garden->tiles[garden->tileSelected].hasPlanter;
}

Planter *garden_getSelectedPlanter(Garden *garden) {
    return &garden->tiles[garden->tileSelected].planter;
}

void garden_update(Garden *garden, float deltaTime, float gameplayTime) {
    garden->lightSourcePos = getLightSourcePosition(garden, gameplayTime);
    updateLightLevelOfTiles(garden);

    for (int i = 0; i < garden->tilesCount; i++) {
        if (garden->tiles[i].hasPlanter && garden->tiles[i].planter.hasPlant) {
            plant_update(&garden->tiles[i].planter.plant, deltaTime);
        }
    }
}

Command garden_processInput(Garden *garden, InputManager *input) {
    Vector2 *mousePos = &input->worldMousePos;
    Vector2 cellHoveredCoords = screenCoordsToGridCoords(garden, mousePos->x, mousePos->y);
    int cellHoveredIndex =
        getPlanterIndexFromCoords(garden, cellHoveredCoords.x, cellHoveredCoords.y);

    garden->tileHovered = cellHoveredIndex;

    if (input->mouseButtonPressed == MOUSE_BUTTON_LEFT) {
        return (Command){COMMAND_TILE_CLICKED, {.tileIndex = cellHoveredIndex}};
    }

    return (Command){COMMAND_NONE};
}

void drawIsoRectangleLines(Garden *garden, IsoRec isoRec, int lineWidth, Color color) {
    DrawLineEx(isoRec.top, isoRec.left, 2, color);
    DrawLineEx(isoRec.left, isoRec.bottom, 2, color);
    DrawLineEx(isoRec.bottom, isoRec.right, 2, color);
    DrawLineEx(isoRec.right, isoRec.top, 2, color);
}

void garden_draw(Garden *garden) {
    for (int i = 0; i < garden->tilesCount; i++) {
        IsoRec isoTile = getPlanterIsoVertices(garden, i);

        if (garden->tileSelected == i || garden->tileHovered == i) {
            Color planterBorderColor = garden->tileSelected == i ? DARKBROWN : BROWN;

            drawIsoRectangleLines(garden, isoTile, 2, planterBorderColor);
        }

        drawIsoRectangleLines(garden, getGardenIsoVertices(garden), 2, WHITE);

        Vector2 tileBase = {isoTile.bottom.x, isoTile.bottom.y};

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
