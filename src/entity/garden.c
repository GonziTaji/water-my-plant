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

typedef struct {
    Vector2 vertices[4];
} RectVertices;

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

    rotated.x += garden->camera.offset.x + TILE_WIDTH;
    rotated.y += garden->camera.offset.y - TILE_HEIGHT;

    return rotated;
}

IsoRec getGardenIsoVertices(Garden *garden) {
    IsoRec isoRec = utils_toIsoRec(&garden->camera,
        (Rectangle){
            0,
            0,
            garden->tileCols,
            garden->tileRows,
        });

    utils_rotateIsoRec(&isoRec, garden->camera.rotation);

    return isoRec;
}

/// If a planter with size NxM is hovered, it's whole area is hovered
IsoRec getHoveredIsoVertices(
    const Garden *garden, int tileIndex, enum GardeningTool tool, int toolVariant) {
    Vector2 origin = utils_grid_getCoordsFromTileIndex(garden->tileCols, tileIndex);
    Vector2 dimensions = {1, 1};

    if (tool == GARDENING_TOOL_PLANTER) {
        dimensions = planter_getDimensions(toolVariant, garden->selectionRotation);
    }

    Rectangle rec = (Rectangle){
        origin.x,
        origin.y,
        dimensions.x,
        dimensions.y,
    };

    IsoRec isoRec = utils_toIsoRec(&garden->camera, rec);
    utils_rotateIsoRec(&isoRec, garden->camera.rotation);

    return isoRec;
}

IsoRec getPlanterIsoVertices(const Garden *garden, int tileIndex) {
    int planterIndex = garden->tiles[tileIndex].planterIndex;
    const Planter *p = &garden->planters[planterIndex];

    IsoRec isoRec = utils_toIsoRec(&garden->camera, p->bounds);
    utils_rotateIsoRec(&isoRec, garden->camera.rotation);

    return isoRec;
}

IsoRec getTileIsoVertices(const Garden *garden, int tileIndex) {
    Vector2 origin = utils_grid_getCoordsFromTileIndex(garden->tileCols, tileIndex);
    Rectangle rec = {origin.x, origin.y, 1, 1};

    IsoRec isoRec = utils_toIsoRec(&garden->camera, rec);
    utils_rotateIsoRec(&isoRec, garden->camera.rotation);

    return isoRec;
}

float distanceBetweenPoints(Vector2 p1, Vector2 p2) {
    return sqrtf(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void updateLightLevelOfTiles(Garden *garden) {
    Vector2 lightSourceInGrid = utils_grid_worldPointToCoords(
        &garden->camera, garden->lightSourcePos.x, garden->lightSourcePos.y);

    for (int i = 0; i < garden->tilesCount; i++) {
        Vector2 tileCoords = utils_grid_getCoordsFromTileIndex(garden->tileCols, i);

        float distance = distanceBetweenPoints(tileCoords, lightSourceInGrid);
        garden->tiles[i].lightLevel = garden->lightSourceLevel - distance;
    }
}

void updateGardenOrigin(Garden *garden) {
    garden->camera.offset = (Vector2){0, 0};

    IsoRec target;

    // TODO: if moved (as in, not centered, update based on scale)
    if (garden->camera.zoom == GARDEN_SCALE_INITIAL) {
        target = getGardenIsoVertices(garden);
    } else if (garden->tileHovered != -1) {
        target = getTileIsoVertices(garden, garden->tileHovered);
    } else if (garden->tileSelected != -1) {
        target = getTileIsoVertices(garden, garden->tileSelected);
    } else {
        target = getGardenIsoVertices(garden);
    }

    garden->camera.offset.x = (garden->screenSize->x - target.right.x - target.left.x) / 2;
    garden->camera.offset.y = (garden->screenSize->y - target.bottom.y - target.top.y) / 2;
}

void garden_init(Garden *garden, Vector2 *screenSize, float gameplayTime) {
    garden->screenSize = screenSize;

    garden->lightSourceLevel = 12;

    garden->tileCols = 10;
    garden->tileRows = 7;
    garden->tilesCount = garden->tileCols * garden->tileRows;
    garden->tileSelected = -1;
    garden->tileHovered = -1;

    garden->selectionRotation = ROTATION_0;

    garden->camera.rotation = ROTATION_0;
    garden->camera.zoom = GARDEN_SCALE_INITIAL;

    updateGardenOrigin(garden);

    for (int i = 0; i < GARDEN_MAX_TILES; i++) {
        planter_empty(&garden->planters[i]);
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

    // planters count is the same as tiles count
    for (int planterIndex = 0; planterIndex < garden->tilesCount; planterIndex++) {
        Planter *planter = &garden->planters[planterIndex];

        if (!planter->exists) {
            continue;
        }

        int plantsCount = planter_getPlantCount(planter);

        for (int plantIndex = 0; planterIndex < plantsCount; planterIndex++) {
            if (planter->plants[plantIndex].exists) {
                plant_update(&planter->plants[plantIndex], deltaTime);
            }
        }
    }
}

void resetGardenScale(Garden *garden) {
    garden->camera.zoom = GARDEN_SCALE_INITIAL;
    updateGardenOrigin(garden);
}

void scaleGarden(Garden *garden, float amount) {
    float newScale = utils_clampf(GARDEN_SCALE_MIN, GARDEN_SCALE_MAX, garden->camera.zoom + amount);

    if (newScale != garden->camera.zoom) {
        garden->camera.zoom = newScale;
        updateGardenOrigin(garden);
    }
}

Message garden_processInput(Garden *garden, InputManager *input) {
    if (input->keyPressed == KEY_EQUAL) {
        scaleGarden(garden, GARDEN_SCALE_STEP);
    } else if (input->keyPressed == KEY_MINUS) {
        scaleGarden(garden, -GARDEN_SCALE_STEP);
    } else if (input->keyPressed == KEY_ZERO) {
        resetGardenScale(garden);
    }

    float mouseWheelMove = GetMouseWheelMove();
    if (mouseWheelMove > 0) {
        scaleGarden(garden, GARDEN_SCALE_STEP);
    } else if (mouseWheelMove < 0) {
        scaleGarden(garden, -GARDEN_SCALE_STEP);
    }

    if (input->keyPressed == KEY_GRAVE) {
        garden->camera.rotation = utils_rotate(garden->camera.rotation, 1);
        updateGardenOrigin(garden);
    }

    if (input->keyPressed == KEY_T) {
        garden->selectionRotation++;

        if (garden->selectionRotation == ROTATION_COUNT) {
            garden->selectionRotation = 0;
        }
    }

    // TODO: delegate detection of mouse button down (not pressed) to input subsystem
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        garden->camera.offset.x -= input->worldMouseDelta.x;
        garden->camera.offset.y -= input->worldMouseDelta.y;

        IsoRec gardenIsoRec = getGardenIsoVertices(garden);
        int isoRecWidth = gardenIsoRec.right.x - gardenIsoRec.left.x;

        int startLimitX = -isoRecWidth + (4 * TILE_WIDTH * garden->camera.zoom);
        int endLimitX = garden->screenSize->x - (4 * TILE_WIDTH * garden->camera.zoom);
        int startLimitY
            = gardenIsoRec.left.y - gardenIsoRec.bottom.y + (4 * TILE_HEIGHT * garden->camera.zoom);
        int endLimitY = garden->screenSize->y - (gardenIsoRec.top.y - gardenIsoRec.left.y)
                      - (4 * TILE_HEIGHT * garden->camera.zoom);

        garden->camera.offset.x = utils_clampf(startLimitX, endLimitX, garden->camera.offset.x);
        garden->camera.offset.y = utils_clampf(startLimitY, endLimitY, garden->camera.offset.y);
    }

    Vector2 *mousePos = &input->worldMousePos;

    Vector2 cellHoveredCoords
        = utils_grid_worldPointToCoords(&garden->camera, mousePos->x, mousePos->y);

    int cellHoveredIndex = utils_grid_getTileIndexFromCoords(
        garden->tileCols, garden->tileRows, cellHoveredCoords.x, cellHoveredCoords.y);

    garden->tileHovered = cellHoveredIndex;

    if (input->mouseButtonPressed == MOUSE_BUTTON_LEFT) {
        return (Message){
            MESSAGE_EV_TILE_CLICKED,
            {.selection = cellHoveredIndex},
        };
    }

    return (Message){MESSAGE_NONE};
}

void drawIsoRectangleLines(Garden *garden, IsoRec isoRec, int lineWidth, Color color) {
    DrawLineEx(isoRec.top, isoRec.left, 2, color);
    DrawLineEx(isoRec.left, isoRec.bottom, 2, color);
    DrawLineEx(isoRec.bottom, isoRec.right, 2, color);
    DrawLineEx(isoRec.right, isoRec.top, 2, color);
}

void garden_draw(Garden *garden, enum GardeningTool toolSelected, int toolVariantSelected) {
    IsoRec hoveredTile = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
    IsoRec selectedTile = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};

    for (int i = 0; i < garden->tilesCount; i++) {
        IsoRec currentTile = getTileIsoVertices(garden, i);

        DrawTextureEx(slab1Texture,
            (Vector2){currentTile.left.x, currentTile.top.y},
            0,
            garden->camera.zoom,
            WHITE);

        if (garden->tileSelected == i || garden->tileHovered == i) {
            int planterIndex = garden->tiles[i].planterIndex;
            bool alive = garden->planters[planterIndex].exists;

            if (alive) {
                currentTile = getPlanterIsoVertices(garden, i);
            } else if (garden->tileHovered == i) {
                currentTile = getHoveredIsoVertices(garden, i, toolSelected, toolVariantSelected);
            }

            if (garden->tileSelected == i) {
                selectedTile = currentTile;
            } else {
                hoveredTile = currentTile;
            }
        }
    }

    // Draw outline of selected and hovered tiles after tiles are drawn
    if (!(hoveredTile.left.x == 0 && hoveredTile.right.x == 0)) {
        drawIsoRectangleLines(garden, hoveredTile, 2, BROWN);
    }

    if (!(selectedTile.left.x == 0 && selectedTile.right.x == 0)) {
        drawIsoRectangleLines(garden, hoveredTile, 2, DARKBROWN);
    }

    drawIsoRectangleLines(garden, getGardenIsoVertices(garden), 2, WHITE);

    int plantsToDrawCount = 0;
    Plant *plantsToDraw[garden->tilesCount];
    Vector2 plantsOrigins[garden->tilesCount];

    for (int i = 0; i < garden->tilesCount; i++) {
        Planter *planter = &garden->planters[i];

        if (!planter->exists) {
            continue;
        }

        int isoTileIndex = utils_grid_getTileIndexFromCoords(
            garden->tileCols, garden->tileRows, planter->bounds.x, planter->bounds.y);
        IsoRec isoTile = getPlanterIsoVertices(garden, isoTileIndex);

        Vector2 isoTileOrigin = (Vector2){isoTile.left.x, isoTile.top.y};

        planter_draw(planter, isoTileOrigin, garden->camera.zoom, garden->camera.rotation, WHITE);

        int plantsCount = planter_getPlantCount(planter);

        for (int j = 0; j < plantsCount; j++) {
            if (planter->plants[j].exists) {
                plantsToDraw[plantsToDrawCount] = &planter->plants[j];
                plantsOrigins[plantsToDrawCount]
                    = planter_getPlantWorldPos(planter, &garden->camera, j);
                plantsToDrawCount++;
            }
        }
    }

    // Draw plants after planters. Maybe use origin's Y base to order in case there's a high planter
    for (int i = 0; i < plantsToDrawCount; i++) {
        plant_draw(plantsToDraw[i], plantsOrigins[i], garden->camera.zoom, WHITE);
    }

    // Draw selection indicator at the end
    if (garden->tileHovered != -1) {
        int i = garden->tileHovered;
        IsoRec isoTile;

        if (toolSelected == GARDENING_TOOL_PLANTER) {
            isoTile = getHoveredIsoVertices(garden, i, toolSelected, toolVariantSelected);
            Vector2 isoTileOrigin = (Vector2){
                isoTile.left.x,
                isoTile.top.y,
            };
            Planter p;
            Vector2 gridCoords = utils_grid_getCoordsFromTileIndex(garden->tileCols, i);
            Vector2 origin
                = utils_grid_coordsToWorldPoint(&garden->camera, gridCoords.x, gridCoords.y);
            planter_init(&p, toolVariantSelected, origin, garden->selectionRotation);
            planter_draw(&p,
                isoTileOrigin,
                garden->camera.zoom,
                garden->camera.rotation,
                (Color){255, 255, 255, 200});
        } else {
            isoTile = getTileIsoVertices(garden, i);
        }

        if (toolSelected == GARDENING_TOOL_PLANT_CUTTING) {
            int planterIndex = garden->tiles[i].planterIndex;
            bool alive = garden->planters[planterIndex].exists;
            float plantBasePosY;

            if (alive) {
                plantBasePosY = garden->planters[planterIndex].plantBasePosY * garden->camera.zoom;
            } else {
                plantBasePosY = TILE_HEIGHT * garden->camera.zoom / 2.0f;
            }

            Vector2 isoTileBase = (Vector2){
                isoTile.bottom.x,
                isoTile.bottom.y - plantBasePosY,
            };
            Plant plant = {.health = 100};
            plant_init(&plant, toolVariantSelected);
            plant_draw(&plant, isoTileBase, garden->camera.zoom, (Color){255, 255, 255, 200});
        }
    }

    DrawCircle(garden->lightSourcePos.x,
        garden->lightSourcePos.y,
        LIGHT_SOURCE_RADIUS * garden->camera.zoom,
        YELLOW);
}
