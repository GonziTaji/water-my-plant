
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
    Vector2 left;
    Vector2 top;
    Vector2 right;
    Vector2 bottom;
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

Vector2 gridCoordsToWorldCoords(const Garden *garden, float x, float y) {
    return (Vector2){
        garden->origin.x + ((x + y) * TILE_WIDTH * WORLD_SCALE / 2.0f),
        garden->origin.y + ((y - x) * TILE_HEIGHT * WORLD_SCALE / 2.0f),
    };
}

Vector2 screenCoordsToGridCoords(const Garden *garden, float x, float y) {
    const float TW = TILE_WIDTH * WORLD_SCALE;
    const float TH = TILE_HEIGHT * WORLD_SCALE;

    const float dx = x - garden->origin.x;
    const float dy = y - garden->origin.y;

    return (Vector2){
        (dx / TW) - (dy / TH),
        (dx / TW) + (dy / TH),
    };
}

bool garden_isValidGridCoords(Garden *garden, float x, float y) {
    if (x < 0 || y < 0 || x >= garden->tileCols || y >= garden->tileRows) {
        return false;
    }

    return true;
}

Vector2 garden_getPlanterCoordsFromIndex(const Garden *garden, int i) {
    return (Vector2){
        i % garden->tileCols,
        (int)(i / garden->tileCols),
    };
}

/** Returns -1 if the coords are not a valid cell of the grid */
int garden_getPlanterIndexFromCoords(Garden *garden, float x, float y) {
    if (!garden_isValidGridCoords(garden, x, y)) {
        return -1;
    }

    return ((int)y * garden->tileCols) + (int)x;
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

IsoRec getPlanterIsoVertices(const Garden *garden, int tileIndex) {
    Vector2 origin = garden_getPlanterCoordsFromIndex(garden, tileIndex);
    Vector2 size = {1, 1};

    int planterIndex = garden->tiles[tileIndex].planterIndex;

    const Planter *p = &garden->planters[planterIndex];

    if (planterIndex != -1 && p->alive) {
        size.x = p->dimensions.x;
        size.y = p->dimensions.y;

        origin.x = p->origin.x;
        origin.y = p->origin.y;
    }

    IsoRec isoRec = {
        gridCoordsToWorldCoords(garden, origin.x, origin.y),
        gridCoordsToWorldCoords(garden, origin.x + size.x, origin.y),
        gridCoordsToWorldCoords(garden, origin.x + size.x, origin.y + size.y),
        gridCoordsToWorldCoords(garden, origin.x, origin.y + size.y),
    };

    return isoRec;
}

float distanceBetweenPoints(Vector2 p1, Vector2 p2) {
    return sqrtf(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void updateLightLevelOfTiles(Garden *garden) {
    Vector2 lightSourceInGrid
        = screenCoordsToGridCoords(garden, garden->lightSourcePos.x, garden->lightSourcePos.y);

    for (int i = 0; i < garden->tilesCount; i++) {
        Vector2 tileCoords = garden_getPlanterCoordsFromIndex(garden, i);

        float distance = distanceBetweenPoints(tileCoords, lightSourceInGrid);
        garden->tiles[i].lightLevel = garden->lightSourceLevel - distance;
    }
}

void garden_init(Garden *garden, Vector2 screenSize, float gameplayTime) {
    garden->lightSourceLevel = 12;
    garden->tileSelected = 0;
    garden->tileHovered = 0;
    garden->tileCols = 10;
    garden->tileRows = 7;
    garden->tilesCount = garden->tileCols * garden->tileRows;

    IsoRec gardenIsoRec = getGardenIsoVertices(garden);

    garden->origin.x = (screenSize.x - gardenIsoRec.right.x - gardenIsoRec.left.x) / 2;
    garden->origin.y = (screenSize.y - gardenIsoRec.bottom.y - gardenIsoRec.top.y) / 2;

    for (int i = 0; i < GARDEN_MAX_TILES; i++) {
        garden->planters[i] = (Planter){
            .type = PLANTER_TYPE_COUNT, // use a specific type to "zero" the value?
            .alive = false,
            .hasPlant = false,
            .dimensions = (Vector2){0, 0},
        };
    }

    for (int i = 0; i < garden->tilesCount; i++) {
        garden->tiles[i].planterIndex = -1;
    }

    garden->lightSourcePos = getLightSourcePosition(garden, gameplayTime);
    updateLightLevelOfTiles(garden);
}

bool garden_hasPlanterSelected(const Garden *garden) {
    return garden->tileSelected != -1 && garden->tiles[garden->tileSelected].planterIndex != -1;
}

Planter *garden_getSelectedPlanter(Garden *garden) {
    int planterIndex = garden->tiles[garden->tileSelected].planterIndex;
    return &garden->planters[planterIndex];
}

void garden_update(Garden *garden, float deltaTime, float gameplayTime) {
    garden->lightSourcePos = getLightSourcePosition(garden, gameplayTime);
    updateLightLevelOfTiles(garden);

    for (int i = 0; i < garden->tilesCount; i++) {
        if (garden->planters[i].alive && garden->planters[i].hasPlant) {
            plant_update(&garden->planters[i].plant, deltaTime);
        }
    }
}

Command garden_processInput(Garden *garden, InputManager *input) {
    Vector2 *mousePos = &input->worldMousePos;
    Vector2 cellHoveredCoords = screenCoordsToGridCoords(garden, mousePos->x, mousePos->y);
    int cellHoveredIndex
        = garden_getPlanterIndexFromCoords(garden, cellHoveredCoords.x, cellHoveredCoords.y);

    garden->tileHovered = cellHoveredIndex;

    if (input->mouseButtonPressed == MOUSE_BUTTON_LEFT) {
        return (Command){
            COMMAND_TILE_CLICKED,
            {.tileIndex = cellHoveredIndex},
        };
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
        if (garden->tileSelected == i || garden->tileHovered == i) {
            IsoRec isoTile = getPlanterIsoVertices(garden, i);

            Color planterBorderColor = garden->tileHovered == i ? BROWN : DARKBROWN;

            drawIsoRectangleLines(garden, isoTile, 2, planterBorderColor);
        }
    }

    // TODO: this is shown over the plants for some reason
    drawIsoRectangleLines(garden, getGardenIsoVertices(garden), 2, WHITE);

    for (int i = 0; i < GARDEN_MAX_TILES; i++) {
        if (garden->planters[i].alive) {
            Planter *planter = &garden->planters[i];

            int isoTileIndex
                = garden_getPlanterIndexFromCoords(garden, planter->origin.x, planter->origin.y);
            IsoRec isoTile = getPlanterIsoVertices(garden, isoTileIndex);

            Vector2 isoTileBase = (Vector2){isoTile.bottom.x, isoTile.bottom.y};

            planter_draw(planter, isoTileBase);

            if (planter->hasPlant) {
                Vector2 plantOrigin = {
                    isoTileBase.x,
                    isoTileBase.y - (planter->plantBasePosY * WORLD_SCALE),
                };
                plant_draw(&planter->plant, plantOrigin);
            };
        }
    }

    DrawCircle(garden->lightSourcePos.x, garden->lightSourcePos.y, LIGHT_SOURCE_RADIUS, YELLOW);
}
