
#include "garden.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "../game/gameplay.h"
#include "../utils/utils.h"
#include "plant.h"
#include "planter.h"
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

IsoRec toIsoRec(const Garden *garden, Rectangle rec) {
    IsoRec isoRec = {
        gridCoordsToWorldCoords(garden, rec.x, rec.y),
        gridCoordsToWorldCoords(garden, rec.x + rec.width, rec.y),
        gridCoordsToWorldCoords(garden, rec.x + rec.width, rec.y + rec.height),
        gridCoordsToWorldCoords(garden, rec.x, rec.y + rec.height),
    };

    rotateIsoRec(&isoRec);

    return isoRec;
}

IsoRec getGardenIsoVertices(Garden *garden) {
    return toIsoRec(garden, (Rectangle){0, 0, garden->tileCols, garden->tileRows});
}

/// If a planter with size NxM is hovered, it's whole area is hovered
IsoRec getHoveredIsoVertices(const Garden *garden, int tileIndex, PlanterType planterTypeSelected) {
    Vector2 origin = garden_getPlanterCoordsFromIndex(garden, tileIndex);
    Vector2 dimensions = planter_getDimensions(planterTypeSelected);

    Rectangle rec = {origin.x, origin.y, dimensions.x, dimensions.y};

    return toIsoRec(garden, rec);
}

IsoRec getPlanterIsoVertices(const Garden *garden, int tileIndex) {
    int planterIndex = garden->tiles[tileIndex].planterIndex;
    const Planter *p = &garden->planters[planterIndex];

    Rectangle rec = {
        p->origin.x,
        p->origin.y,
        p->dimensions.x,
        p->dimensions.y,
    };

    return toIsoRec(garden, rec);
}

IsoRec getTileIsoVertices(const Garden *garden, int tileIndex) {
    Vector2 origin = garden_getPlanterCoordsFromIndex(garden, tileIndex);
    Rectangle rec = {origin.x, origin.y, 1, 1};

    return toIsoRec(garden, rec);
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

void updateGardenOrigin(Garden *garden) {
    garden->origin = (Vector2){0, 0};

    IsoRec target;

    // TODO: if moved (as in, not centered, update based on scale)
    if (gardenScale == GARDEN_SCALE_INITIAL) {
        target = getGardenIsoVertices(garden);
    } else if (garden->tileHovered != -1) {
        target = getTileIsoVertices(garden, garden->tileHovered);
    } else if (garden->tileSelected != -1) {
        target = getTileIsoVertices(garden, garden->tileSelected);
    } else {
        target = getGardenIsoVertices(garden);
    }

    garden->origin.x = (garden->screenSize->x - target.right.x - target.left.x) / 2;
    garden->origin.y = (garden->screenSize->y - target.bottom.y - target.top.y) / 2;
}

void garden_init(Garden *garden, Vector2 *screenSize, float gameplayTime) {
    garden->screenSize = screenSize;
    garden->lightSourceLevel = 12;
    garden->tileSelected = -1;
    garden->tileHovered = -1;
    garden->tileCols = 10;
    garden->tileRows = 7;
    garden->tilesCount = garden->tileCols * garden->tileRows;

    updateGardenOrigin(garden);

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
    updateGardenOrigin(garden);
}

void scaleGarden(Garden *garden, float amount) {
    gardenScale = utils_clampf(GARDEN_SCALE_MIN, GARDEN_SCALE_MAX, gardenScale + amount);
    updateGardenOrigin(garden);
}

/// rotates garden clockwise
void rotateGarden(Garden *garden) {
    gardenRotation++;

    if (gardenRotation == GARDEN_ROTATION_COUNT) {
        gardenRotation = 0;
    }

    updateGardenOrigin(garden);
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

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        garden->origin.x -= input->worldMouseDelta.x;
        garden->origin.y -= input->worldMouseDelta.y;

        IsoRec gardenIsoRec = getGardenIsoVertices(garden);
        int isoRecWidth = gardenIsoRec.right.x - gardenIsoRec.left.x;

        int startLimitX = -isoRecWidth + (4 * TILE_WIDTH * gardenScale);
        int endLimitX = garden->screenSize->x - (4 * TILE_WIDTH * gardenScale);
        int startLimitY
            = gardenIsoRec.left.y - gardenIsoRec.bottom.y + (4 * TILE_HEIGHT * gardenScale);
        int endLimitY = garden->screenSize->y - (gardenIsoRec.top.y - gardenIsoRec.left.y)
                      - (4 * TILE_HEIGHT * gardenScale);

        garden->origin.x = utils_clampf(startLimitX, endLimitX, garden->origin.x);
        garden->origin.y = utils_clampf(startLimitY, endLimitY, garden->origin.y);
    }

    Vector2 *mousePos = &input->worldMousePos;
    Vector2 cellHoveredCoords = screenCoordsToGridCoords(garden, mousePos->x, mousePos->y);
    int cellHoveredIndex
        = garden_getPlanterIndexFromCoords(garden, cellHoveredCoords.x, cellHoveredCoords.y);

    garden->tileHovered = cellHoveredIndex;

    if (input->mouseButtonPressed == MOUSE_BUTTON_LEFT) {
        return (Command){
            COMMAND_TILE_CLICKED,
            {.selection = cellHoveredIndex},
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

void garden_draw(Garden *garden, enum GardeningTool toolSelected, int toolVariantSelected) {
    for (int i = 0; i < garden->tilesCount; i++) {
        if (garden->tileSelected == i || garden->tileHovered == i) {
            IsoRec isoTile;

            int planterIndex = garden->tiles[i].planterIndex;
            bool alive = garden->planters[planterIndex].alive;

            if (alive) {
                isoTile = getPlanterIsoVertices(garden, i);
            } else if (garden->tileHovered == i && toolSelected == GARDENING_TOOL_PLANTER) {
                isoTile = getHoveredIsoVertices(garden, i, toolVariantSelected);
            } else {
                isoTile = getTileIsoVertices(garden, i);
            }

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
