
#include "garden.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "../utils/utils.h"
#include "plant.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stddef.h>

#define LIGHT_SOURCE_RADIUS 40
#define TILE_WIDTH 128
#define TILE_HEIGHT (int)(TILE_WIDTH / 2)
#define GARDEN_SCALE_INITIAL 1.0f
#define GARDEN_SCALE_STEP 0.4f
#define GARDEN_SCALE_MIN GARDEN_SCALE_INITIAL - (1 * GARDEN_SCALE_STEP)
#define GARDEN_SCALE_MAX GARDEN_SCALE_INITIAL + (4 * GARDEN_SCALE_STEP)

typedef enum {
    GARDEN_ROTATION_INITIAL,
    GARDEN_ROTATION_90,
    GARDEN_ROTATION_180,
    GARDEN_ROTATION_270,
    GARDEN_ROTATION_COUNT,
} GardenRotation;

GardenRotation gardenRotation = GARDEN_ROTATION_INITIAL;
float gardenScale = GARDEN_SCALE_INITIAL;

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
    int sumX = 0;
    int sumY = 0;

    // TODO: refactor in some way? ugly
    switch (gardenRotation) {
    case GARDEN_ROTATION_INITIAL:
        sumX = +x + y;
        sumY = -x + y;
        break;
    case GARDEN_ROTATION_90:
        sumX = +x - y;
        sumY = +x + y;
        break;
    case GARDEN_ROTATION_180:
        sumX = -x - y;
        sumY = +x - y;
        break;
    case GARDEN_ROTATION_270:
        sumX = -x + y;
        sumY = -x - y;
        break;
    case GARDEN_ROTATION_COUNT:
        // Handle in some way? give rotation initial vector?
        break;
    }

    return (Vector2){
        garden->origin.x + (sumX * TILE_WIDTH * gardenScale / 2.0f),
        garden->origin.y + (sumY * TILE_HEIGHT * gardenScale / 2.0f),
    };
}

Vector2 screenCoordsToGridCoords(const Garden *garden, float x, float y) {
    const float TW = TILE_WIDTH * gardenScale;
    const float TH = TILE_HEIGHT * gardenScale;

    const float dx = x - garden->origin.x;
    const float dy = y - garden->origin.y;

    Vector2 gridCoords = {};

    // TODO: refactor in some way? ugly
    switch (gardenRotation) {
    case GARDEN_ROTATION_INITIAL:
        gridCoords.x = +(dx / TW) - (dy / TH);
        gridCoords.y = +(dx / TW) + (dy / TH);
        break;
    case GARDEN_ROTATION_90:
        gridCoords.x = +(dx / TW) + (dy / TH);
        gridCoords.y = -(dx / TW) + (dy / TH);
        break;
    case GARDEN_ROTATION_180:
        gridCoords.x = -(dx / TW) + (dy / TH);
        gridCoords.y = -(dx / TW) - (dy / TH);
        break;
    case GARDEN_ROTATION_270:
        gridCoords.x = -(dx / TW) - (dy / TH);
        gridCoords.y = +(dx / TW) - (dy / TH);
        break;
    case GARDEN_ROTATION_COUNT:
        // Handle in some way? give rotation initial vector?
        assert(false);
        break;
    }

    return gridCoords;
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

/// Relabels each vertice depending on the rotation of the view
void rotateIsoRec(IsoRec *isoRec) {
    Vector2 source[4] = {
        isoRec->left,
        isoRec->top,
        isoRec->right,
        isoRec->bottom,
    };

    int rotatedIndex;
    for (int i = 0; i < GARDEN_ROTATION_COUNT; i++) {
        rotatedIndex = i + gardenRotation;

        if (rotatedIndex >= GARDEN_ROTATION_COUNT) {
            rotatedIndex -= 4;
        }

        switch (rotatedIndex) {
        case 0:
            isoRec->left = source[i];
            break;
        case 1:
            isoRec->top = source[i];
            break;
        case 2:
            isoRec->right = source[i];
            break;
        case 3:
            isoRec->bottom = source[i];
            break;
        }
    }
}

IsoRec getGardenIsoVertices(Garden *garden) {
    IsoRec isoRec = {
        gridCoordsToWorldCoords(garden, 0, 0),
        gridCoordsToWorldCoords(garden, garden->tileCols, 0),
        gridCoordsToWorldCoords(garden, garden->tileCols, garden->tileRows),
        gridCoordsToWorldCoords(garden, 0, garden->tileRows),
    };

    rotateIsoRec(&isoRec);

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

    rotateIsoRec(&isoRec);

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

void updateOriginToScreenCenter(Garden *garden) {
    garden->origin = (Vector2){0, 0};
    IsoRec gardenIsoRec = getGardenIsoVertices(garden);

    garden->origin.x = (garden->screenSize->x - gardenIsoRec.right.x - gardenIsoRec.left.x) / 2;
    garden->origin.y = (garden->screenSize->y - gardenIsoRec.bottom.y - gardenIsoRec.top.y) / 2;
}

void garden_init(Garden *garden, Vector2 *screenSize, float gameplayTime) {
    garden->screenSize = screenSize;
    garden->lightSourceLevel = 12;
    garden->tileSelected = 0;
    garden->tileHovered = 0;
    garden->tileCols = 10;
    garden->tileRows = 7;
    garden->tilesCount = garden->tileCols * garden->tileRows;

    updateOriginToScreenCenter(garden);

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

void resetGardenScale(Garden *garden) {
    gardenScale = GARDEN_SCALE_INITIAL;
    updateOriginToScreenCenter(garden);
}

void scaleGarden(Garden *garden, float amount) {
    gardenScale = utils_clampf(GARDEN_SCALE_MIN, GARDEN_SCALE_MAX, gardenScale + amount);
    updateOriginToScreenCenter(garden);
    updateOriginToScreenCenter(garden);
}

/// rotates garden clockwise
void rotateGarden(Garden *garden) {
    gardenRotation++;

    if (gardenRotation == GARDEN_ROTATION_COUNT) {
        gardenRotation = 0;
    }

    updateOriginToScreenCenter(garden);
}

Command garden_processInput(Garden *garden, InputManager *input) {
    if (input->keyPressed == KEY_EQUAL) {
        scaleGarden(garden, GARDEN_SCALE_STEP);
    } else if (input->keyPressed == KEY_MINUS) {
        scaleGarden(garden, -GARDEN_SCALE_STEP);
    } else if (input->keyPressed == KEY_ZERO) {
        resetGardenScale(garden);
    }

    if (input->keyPressed == KEY_ONE) {
        rotateGarden(garden);
    }

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

    drawIsoRectangleLines(garden, getGardenIsoVertices(garden), 2, WHITE);

    int plantsCount = 0;
    Plant *plantsToDraw[garden->tilesCount];
    Vector2 plantsOrigins[garden->tilesCount];

    for (int i = 0; i < garden->tilesCount; i++) {
        if (garden->planters[i].alive) {
            Planter *planter = &garden->planters[i];

            int isoTileIndex
                = garden_getPlanterIndexFromCoords(garden, planter->origin.x, planter->origin.y);
            IsoRec isoTile = getPlanterIsoVertices(garden, isoTileIndex);

            Vector2 isoTileBase = (Vector2){isoTile.bottom.x, isoTile.bottom.y};

            planter_draw(planter, isoTileBase, gardenScale);

            if (planter->hasPlant) {
                plantsToDraw[plantsCount] = &planter->plant;
                plantsOrigins[plantsCount] = (Vector2){
                    isoTileBase.x,
                    isoTileBase.y - (planter->plantBasePosY * gardenScale),
                };
                plantsCount++;
            };
        }
    }

    // Draw plants after planters. Maybe use origin's Y base to order in case there's a high planter
    for (int i = 0; i < plantsCount; i++) {
        plant_draw(plantsToDraw[i], plantsOrigins[i], gardenScale);
    }

    DrawCircle(garden->lightSourcePos.x,
        garden->lightSourcePos.y,
        LIGHT_SOURCE_RADIUS * gardenScale,
        YELLOW);
}
