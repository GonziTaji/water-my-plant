#include "garden.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "../game/gameplay.h"
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
    v.x *= (garden->tileGrid.cols) * TILE_WIDTH;
    v.y *= maxHeight;

    float rad = 25 * (M_PI / 180);

    Vector2 rotated = {
        v.x * cos(rad) - v.y * sin(rad),
        v.y * cos(rad) + v.x * sin(rad),
    };

    rotated.x += garden->transform.translation.x + TILE_WIDTH;
    rotated.y += garden->transform.translation.y - TILE_HEIGHT;

    return rotated;
}

IsoRec getGardenIsoVertices(Garden *garden) {
    IsoRec isoRec = utils_toIsoRec(&garden->transform,
        (Rectangle){
            0,
            0,
            garden->tileGrid.cols,
            garden->tileGrid.rows,
        });

    return isoRec;
}

/// If a planter with size NxM is hovered, it's whole area is hovered
IsoRec getHoveredIsoVertices(
    const Garden *garden, int tileIndex, enum GardeningTool tool, int toolVariant) {
    Vector2 translation = utils_grid_getCoordsFromTileIndex(garden->tileGrid.cols, tileIndex);
    Vector2 dimensions = {1, 1};

    if (tool == GARDENING_TOOL_PLANTER) {
        dimensions = planter_getDimensions(toolVariant, garden->selectionRotation);
    }

    Rectangle rec = (Rectangle){
        translation.x,
        translation.y,
        dimensions.x,
        dimensions.y,
    };

    IsoRec isoRec = utils_toIsoRec(&garden->transform, rec);

    return isoRec;
}

IsoRec getPlanterIsoVertices(const Garden *garden, int tileIndex) {
    int planterIndex = garden->tiles[tileIndex].planterIndex;
    const Planter *p = &garden->planters[planterIndex];

    IsoRec isoRec = utils_toIsoRec(&garden->transform, p->bounds);

    return isoRec;
}

IsoRec getTileIsoVertices(const Garden *garden, int tileIndex) {
    Vector2 translation = utils_grid_getCoordsFromTileIndex(garden->tileGrid.cols, tileIndex);
    Rectangle rec = {translation.x, translation.y, 1, 1};

    IsoRec isoRec = utils_toIsoRec(&garden->transform, rec);

    return isoRec;
}

float distanceBetweenPoints(Vector2 p1, Vector2 p2) {
    return sqrtf(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void updateLightLevelOfTiles(Garden *garden) {
    Vector2 lightSourceInGrid = utils_grid_worldPointToCoords(
        &garden->transform, garden->lightSourcePos.x, garden->lightSourcePos.y);

    for (int i = 0; i < garden->tileGrid.tileCount; i++) {
        Vector2 tileCoords = utils_grid_getCoordsFromTileIndex(garden->tileGrid.cols, i);

        float distance = distanceBetweenPoints(tileCoords, lightSourceInGrid);
        garden->tiles[i].lightLevel = garden->lightSourceLevel - distance;
    }
}

void updateGardenOrigin(Garden *garden) {
    garden->transform.translation = (Vector2){0, 0};

    IsoRec target;

    // TODO: if moved (as in, not centered, update based on scale)
    if (garden->transform.scale == GARDEN_SCALE_INITIAL) {
        target = getGardenIsoVertices(garden);
    } else if (garden->tileHovered != -1) {
        target = getTileIsoVertices(garden, garden->tileHovered);
    } else if (garden->tileSelected != -1) {
        target = getTileIsoVertices(garden, garden->tileSelected);
    } else {
        target = getGardenIsoVertices(garden);
    }

    garden->transform.translation.x = (garden->screenSize->x - target.right.x - target.left.x) / 2;
    garden->transform.translation.y = (garden->screenSize->y - target.bottom.y - target.top.y) / 2;
}

// TODO: move
int grid_getTilesCount(TileGrid *grid) {
    return grid->cols * grid->rows;
}

void garden_init(Garden *garden, Vector2 *screenSize, float gameplayTime) {
    garden->screenSize = screenSize;

    garden->lightSourceLevel = 12;

    garden->tileGrid = (TileGrid){10, 7, TILE_WIDTH, TILE_HEIGHT};
    garden->tileGrid.tileCount = garden->tileGrid.cols * garden->tileGrid.rows;

    garden->tileSelected = -1;
    garden->tileHovered = -1;

    garden->selectionRotation = ROTATION_0;
    garden->transform.rotation = ROTATION_0;
    garden->transform.scale = GARDEN_SCALE_INITIAL;
    garden->transform.translation.x = 0;
    garden->transform.translation.y = 0;

    updateGardenOrigin(garden);

    for (int i = 0; i < GARDEN_MAX_TILES; i++) {
        planter_empty(&garden->planters[i]);
    }

    for (int i = 0; i < garden->tileGrid.tileCount; i++) {
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
    for (int planterIndex = 0; planterIndex < garden->tileGrid.tileCount; planterIndex++) {
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
    garden->transform.scale = GARDEN_SCALE_INITIAL;
    updateGardenOrigin(garden);
}

void scaleGarden(Garden *garden, float amount) {
    float newScale
        = utils_clampf(GARDEN_SCALE_MIN, GARDEN_SCALE_MAX, garden->transform.scale + amount);

    if (newScale != garden->transform.scale) {
        garden->transform.scale = newScale;
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
        garden->transform.rotation = utils_rotate(garden->transform.rotation, 1);
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
        garden->transform.translation.x -= input->worldMouseDelta.x;
        garden->transform.translation.y -= input->worldMouseDelta.y;

        IsoRec gardenIsoRec = getGardenIsoVertices(garden);
        int isoRecWidth = gardenIsoRec.right.x - gardenIsoRec.left.x;

        int startLimitX = -isoRecWidth + (4 * TILE_WIDTH * garden->transform.scale);
        int endLimitX = garden->screenSize->x - (4 * TILE_WIDTH * garden->transform.scale);
        int startLimitY = gardenIsoRec.left.y - gardenIsoRec.bottom.y
                        + (4 * TILE_HEIGHT * garden->transform.scale);
        int endLimitY = garden->screenSize->y - (gardenIsoRec.top.y - gardenIsoRec.left.y)
                      - (4 * TILE_HEIGHT * garden->transform.scale);

        garden->transform.translation.x
            = utils_clampf(startLimitX, endLimitX, garden->transform.translation.x);
        garden->transform.translation.y
            = utils_clampf(startLimitY, endLimitY, garden->transform.translation.y);
    }

    Vector2 *mousePos = &input->worldMousePos;

    Vector2 cellHoveredCoords
        = utils_grid_worldPointToCoords(&garden->transform, mousePos->x, mousePos->y);

    int cellHoveredIndex = utils_grid_getTileIndexFromCoords(
        garden->tileGrid.cols, garden->tileGrid.rows, cellHoveredCoords.x, cellHoveredCoords.y);

    garden->tileHovered = cellHoveredIndex;

    if (input->mouseButtonPressed == MOUSE_BUTTON_LEFT) {
        return (Message){MESSAGE_EV_TILE_CLICKED, {.selection = cellHoveredIndex}};
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

    for (int i = 0; i < garden->tileGrid.tileCount; i++) {
        IsoRec currentTile = getTileIsoVertices(garden, i);

        DrawTextureEx(slab1Texture,
            (Vector2){currentTile.left.x, currentTile.top.y},
            0,
            garden->transform.scale,
            WHITE);

        if (garden->tileSelected == i || garden->tileHovered == i) {
            int planterIndex = garden->tiles[i].planterIndex;
            bool alive = garden->planters[planterIndex].exists;

            if (planterIndex != -1 && alive) {
                currentTile = getPlanterIsoVertices(garden, i);
            } else if (toolSelected == GARDENING_TOOL_PLANTER) {
                Vector2 p = utils_grid_getCoordsFromTileIndex(garden->tileGrid.cols, i);
                Vector2 d = planter_getDimensions(toolVariantSelected, garden->transform.rotation);

                currentTile = utils_toIsoRec(&garden->transform, (Rectangle){p.x, p.y, d.x, d.y});
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

        BeginBlendMode(BLEND_ADDITIVE);

        DrawTriangle(
            hoveredTile.left, hoveredTile.bottom, hoveredTile.right, (Color){255, 255, 255, 40});
        DrawTriangle(
            hoveredTile.right, hoveredTile.top, hoveredTile.left, (Color){255, 255, 255, 40});
        EndBlendMode();
    }

    if (!(selectedTile.left.x == 0 && selectedTile.right.x == 0)) {
        drawIsoRectangleLines(garden, hoveredTile, 2, DARKBROWN);
    }

    drawIsoRectangleLines(garden, getGardenIsoVertices(garden), 2, WHITE);

    int plantsToDrawCount = 0;
    Plant *plantsToDraw[garden->tileGrid.tileCount];
    Vector2 plantsOrigins[garden->tileGrid.tileCount];

    for (int i = 0; i < garden->tileGrid.tileCount; i++) {
        Planter *planter = &garden->planters[i];

        if (!planter->exists) {
            continue;
        }

        int isoTileIndex = utils_grid_getTileIndexFromCoords(
            garden->tileGrid.cols, garden->tileGrid.rows, planter->bounds.x, planter->bounds.y);
        IsoRec isoTile = getPlanterIsoVertices(garden, isoTileIndex);

        Vector2 isoTileOrigin = (Vector2){isoTile.left.x, isoTile.top.y};

        planter_draw(
            planter, isoTileOrigin, garden->transform.scale, garden->transform.rotation, WHITE);

        int tileIndex = utils_grid_getTileIndexFromCoords(
            garden->tileGrid.cols, garden->tileGrid.cols, planter->bounds.x, planter->bounds.y);

        if (tileIndex == garden->tileHovered) {
            BeginBlendMode(BLEND_ADDITIVE);
            planter_draw(planter,
                isoTileOrigin,
                garden->transform.scale,
                garden->transform.rotation,
                (Color){255, 255, 255, 100});
            EndBlendMode();
        }

        int plantsCount = planter_getPlantCount(planter);

        for (int j = 0; j < plantsCount; j++) {
            if (planter->plants[j].exists) {
                plantsToDraw[plantsToDrawCount] = &planter->plants[j];
                plantsOrigins[plantsToDrawCount]
                    = planter_getPlantWorldPos(planter, &garden->transform, j);
                plantsToDrawCount++;
            }
        }
    }

    // Draw plants after planters. Maybe use translation's Y base to order in case there's a high
    // planter
    for (int i = 0; i < plantsToDrawCount; i++) {
        plant_draw(plantsToDraw[i], plantsOrigins[i], garden->transform.scale, WHITE);
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
            Vector2 gridCoords = utils_grid_getCoordsFromTileIndex(garden->tileGrid.cols, i);
            Vector2 translation
                = utils_grid_coordsToWorldPoint(&garden->transform, gridCoords.x, gridCoords.y);
            planter_init(&p, toolVariantSelected, translation, garden->selectionRotation);
            planter_draw(&p,
                isoTileOrigin,
                garden->transform.scale,
                garden->transform.rotation,
                (Color){255, 255, 255, 200});
        } else {
            isoTile = getTileIsoVertices(garden, i);
        }

        if (toolSelected == GARDENING_TOOL_PLANT_CUTTING) {
            int planterIndex = garden->tiles[i].planterIndex;
            bool alive = garden->planters[planterIndex].exists;
            float plantBasePosY;

            if (alive) {
                plantBasePosY
                    = garden->planters[planterIndex].plantBasePosY * garden->transform.scale;
            } else {
                plantBasePosY = TILE_HEIGHT * garden->transform.scale / 2.0f;
            }

            Vector2 isoTileBase = (Vector2){
                isoTile.bottom.x,
                isoTile.bottom.y - plantBasePosY,
            };
            Plant plant = {.health = 100};
            plant_init(&plant, toolVariantSelected);
            plant_draw(&plant, isoTileBase, garden->transform.scale, (Color){255, 255, 255, 200});
        }
    }

    DrawCircle(garden->lightSourcePos.x,
        garden->lightSourcePos.y,
        LIGHT_SOURCE_RADIUS * garden->transform.scale,
        YELLOW);
}
