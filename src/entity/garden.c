#include "garden.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "../game/gameplay.h"
#include "plant.h"
#include "planter.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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
    IsoRec isoRec = grid_toIsoRec(&garden->transform,
        (Vector2){0, 0},
        (Vector2){garden->tileGrid.cols, garden->tileGrid.rows},
        garden->tileGrid.tileWidth,
        garden->tileGrid.tileHeight);

    return isoRec;
}

IsoRec getHoveredIsoVertices(
    const Garden *garden, int tileIndex, enum GardeningTool tool, int toolVariant) {

    Vector2 coords = grid_getCoordsFromTileIndex(garden->tileGrid.cols, tileIndex);
    Vector2 dimensions = {1, 1};

    if (tool == GARDENING_TOOL_PLANTER) {
        dimensions = planter_getFootPrint(toolVariant, garden->selectionRotation);

    } else if (tool == GARDENING_TOOL_NONE && garden->planterPickedUpIndex != -1) {
        const Planter *p = &garden->planters[garden->planterPickedUpIndex];
        dimensions = planter_getFootPrint(p->type, garden->selectionRotation);
    }

    IsoRec isoRec = grid_toIsoRec(&garden->transform,
        coords,
        dimensions,
        garden->tileGrid.tileWidth,
        garden->tileGrid.tileHeight);

    return isoRec;
}

IsoRec getPlanterIsoVertices(const Garden *garden, int tileIndex) {
    int planterIndex = garden->tiles[tileIndex].planterIndex;
    const Planter *p = &garden->planters[planterIndex];

    IsoRec isoRec = grid_toIsoRec(&garden->transform,
        p->coords,
        planter_getFootPrint(p->type, p->rotation),
        garden->tileGrid.tileWidth,
        garden->tileGrid.tileHeight);

    return isoRec;
}

IsoRec getTileIsoVertices(const Garden *garden, int tileIndex) {
    Vector2 coords = grid_getCoordsFromTileIndex(garden->tileGrid.cols, tileIndex);

    IsoRec isoRec = grid_toIsoRec(&garden->transform,
        coords,
        (Vector2){1, 1},
        garden->tileGrid.tileWidth,
        garden->tileGrid.tileHeight);

    return isoRec;
}

float distanceBetweenPoints(Vector2 p1, Vector2 p2) {
    return sqrtf(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void updateLightLevelOfTiles(Garden *garden) {
    Vector2 lightSourceInGrid = grid_worldPointToCoords(&garden->transform,
        garden->lightSourcePos.x,
        garden->lightSourcePos.y,
        garden->tileGrid.tileWidth,
        garden->tileGrid.tileHeight);

    for (int i = 0; i < garden->tileGrid.tileCount; i++) {
        Vector2 tileCoords = grid_getCoordsFromTileIndex(garden->tileGrid.cols, i);

        float distance = distanceBetweenPoints(tileCoords, lightSourceInGrid);
        garden->tiles[i].lightLevel = garden->lightSourceLevel - distance;
    }
}

void garden_updateGardenOrigin(Garden *garden) {
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
    garden->planterPickedUpIndex = -1;
    garden->screenSize = screenSize;

    garden->lightSourceLevel = 12;

    garden->tileGrid = (TileGrid){14, 12, TILE_WIDTH, TILE_HEIGHT};
    garden->tileGrid.tileCount = garden->tileGrid.cols * garden->tileGrid.rows;

    garden->tileSelected = -1;
    garden->tileHovered = -1;
    garden->planterTileHovered = -1;

    garden->selectionRotation = ROTATION_0;
    garden->transform.rotation = ROTATION_0;
    garden->transform.scale = GARDEN_SCALE_INITIAL;
    garden->transform.translation.x = 0;
    garden->transform.translation.y = 0;

    garden_updateGardenOrigin(garden);

    for (int i = 0; i < GARDEN_MAX_TILES; i++) {
        planter_empty(&garden->planters[i]);
    }

    for (int i = 0; i < garden->tileGrid.tileCount; i++) {
        garden->tiles[i].planterIndex = -1;
    }

    garden->lightSourcePos = getLightSourcePosition(garden, gameplayTime);
    updateLightLevelOfTiles(garden);
}

Vector2 garden_getTileOrigin(Garden *garden, Vector2 coords) {
    return grid_coordsToWorldPoint(&garden->transform,
        coords.x,
        coords.y,
        garden->tileGrid.tileWidth,
        garden->tileGrid.tileHeight);
}

bool garden_hasPlanterSelected(const Garden *garden) {
    int planterIndex = garden->tiles[garden->tileSelected].planterIndex;

    return garden->tileSelected != -1 && planterIndex != -1
        && garden->planters[planterIndex].exists;
}

Planter *garden_getSelectedPlanter(Garden *garden) {
    if (!garden_hasPlanterSelected(garden)) {
        return NULL;
    }

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

        int plantsCount = planter->plantGrid.tileCount;

        for (int plantIndex = 0; planterIndex < plantsCount; planterIndex++) {
            if (planter->plants[plantIndex].exists) {
                plant_update(&planter->plants[plantIndex], deltaTime);
            }
        }
    }
}

Message garden_processInput(Garden *garden, InputManager *input) {
    Vector2 *mousePos = &input->worldMousePos;

    Vector2 tileHoveredCoords = grid_worldPointToCoords(&garden->transform,
        mousePos->x,
        mousePos->y,
        garden->tileGrid.tileWidth,
        garden->tileGrid.tileHeight);

    int tileHoveredIndex = grid_getTileIndexFromCoords(
        garden->tileGrid.cols, garden->tileGrid.rows, tileHoveredCoords.x, tileHoveredCoords.y);

    garden->tileHovered = tileHoveredIndex;

    garden->planterTileHovered = -1;

    if (garden->tileHovered != -1 && garden->tiles[garden->tileHovered].planterIndex != -1) {
        int planterIndex = garden->tiles[garden->tileHovered].planterIndex;

        if (planterIndex != -1) {
            Planter *planter = &garden->planters[planterIndex];

            if (planter->exists) {
                Vector2 planterOrigin = garden_getTileOrigin(garden, planter->coords);

                garden->planterTileHovered = planter_getPlantIndexFromWorldPos(
                    planter, &garden->transform, planterOrigin, input->worldMousePos);
            }
        }
    }

    if (tileHoveredIndex != -1
        && input->mouseButtonState[MOUSE_BUTTON_LEFT] == MOUSE_BUTTON_STATE_PRESSED) {

        return (Message){MESSAGE_EV_TILE_CLICKED, {.selection = tileHoveredIndex}};
    }

    return (Message){MESSAGE_NONE};
}

void drawIsoRectangleLines(Garden *garden, IsoRec isoRec, int lineWidth, Color color) {
    DrawLineEx(isoRec.top, isoRec.left, 2, color);
    DrawLineEx(isoRec.left, isoRec.bottom, 2, color);
    DrawLineEx(isoRec.bottom, isoRec.right, 2, color);
    DrawLineEx(isoRec.right, isoRec.top, 2, color);
}

typedef enum {
    DRAWABLE_PLANTER,
    DRAWABLE_PLANT,
} DrawableType;

typedef struct {
    DrawableType type;
    void *data;
    Vector2 origin;
    int tileDepth;
    int localDepth;
    bool highlight;
    bool pickedUp;
} Drawable;

int compareDrawableDepths(const void *a, const void *b) {
    const Drawable *da = a;
    const Drawable *db = b;

    if (da->tileDepth != db->tileDepth) {
        return da->tileDepth - db->tileDepth;
    }

    return da->localDepth - db->localDepth;
}

int getPlantZIndex(Rotation gardenRotation, Planter *planter, int plantIndex) {
    Vector2 plantCoords = grid_getCoordsFromTileIndex(planter->plantGrid.cols, plantIndex);

    int zIndex = grid_getZIndex(gardenRotation,
        plantCoords.x,
        plantCoords.y,
        planter->plantGrid.cols,
        planter->plantGrid.rows);

    return zIndex;
}

int getPlanterZIndex(Garden *garden, int planterTileIndex) {
    IsoRec planterIsoRec = getPlanterIsoVertices(garden, planterTileIndex);
    Vector2 nearestTileCoords = grid_worldPointToCoords(&garden->transform,
        planterIsoRec.right.x - 1,
        planterIsoRec.right.y,
        garden->tileGrid.tileWidth,
        garden->tileGrid.tileHeight);

    int zIndex = grid_getZIndex(garden->transform.rotation,
        nearestTileCoords.x,
        nearestTileCoords.y,
        garden->tileGrid.cols,
        garden->tileGrid.rows);

    return zIndex;
}

void garden_draw(Garden *garden, enum GardeningTool toolSelected, int toolVariantSelected) {
    assert(garden->tileGrid.tileWidth > 0);
    assert(garden->tileGrid.tileHeight > 0);
    IsoRec hoveredTile = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
    IsoRec selectedTile = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};

    int maxEntities
        = garden->tileGrid.tileCount + (garden->tileGrid.tileCount * PLANTER_MAX_PLANTS);

    Drawable entitiesToDraw[maxEntities];
    int entitiesToDrawCount = 0;

    // Draw tiles and identify the hovered and selected tile
    for (int i = 0; i < garden->tileGrid.tileCount; i++) {
        IsoRec currentTile = getTileIsoVertices(garden, i);

        DrawTexturePro(slab1Texture,
            (Rectangle){0, 0, slab1Texture.width, slab1Texture.height},
            (Rectangle){
                currentTile.left.x,
                currentTile.top.y,
                garden->tileGrid.tileWidth * garden->transform.scale,
                garden->tileGrid.tileHeight * garden->transform.scale,
            },
            (Vector2){0, 0},
            0,
            WHITE);

        if (garden->tileSelected == i || garden->tileHovered == i) {
            int planterIndex = garden->tiles[i].planterIndex;

            if (garden->tileHovered == i) {
                currentTile = getHoveredIsoVertices(garden, i, toolSelected, toolVariantSelected);
            } else if (planterIndex != -1 && garden->planters[planterIndex].exists) {
                currentTile = getPlanterIsoVertices(garden, i);
            }

            if (garden->tileSelected == i) {
                selectedTile = currentTile;
            }

            if (garden->tileHovered == i) {
                hoveredTile = currentTile;
            }
        }
    }

    // Draw garden outline
    drawIsoRectangleLines(garden, getGardenIsoVertices(garden), 4, WHITE);

    // Draw selection/hovered indicators
    if (!(selectedTile.left.x == 0 && selectedTile.right.x == 0)) {
        drawIsoRectangleLines(garden, selectedTile, 2, DARKBROWN);
    }

    if (!(hoveredTile.left.x == 0 && hoveredTile.right.x == 0)) {
        BeginBlendMode(BLEND_ADDITIVE);

        DrawTriangle(
            hoveredTile.left, hoveredTile.bottom, hoveredTile.right, (Color){255, 255, 255, 40});
        DrawTriangle(
            hoveredTile.right, hoveredTile.top, hoveredTile.left, (Color){255, 255, 255, 40});
        EndBlendMode();
    }

    GardenTile *tileHovered = &garden->tiles[garden->tileHovered];

    // Get all the entities to draw
    for (int i = 0; i < garden->tileGrid.tileCount; i++) {

        Planter *planter = &garden->planters[i];

        if (planter->exists) {
            int isoTileIndex = grid_getTileIndexFromCoords(
                garden->tileGrid.cols, garden->tileGrid.rows, planter->coords.x, planter->coords.y);

            IsoRec isoTile = getPlanterIsoVertices(garden, isoTileIndex);
            Vector2 isoTileOrigin = (Vector2){isoTile.left.x, isoTile.top.y};

            int zIndex = getPlanterZIndex(garden, isoTileIndex);
            bool pickedUp = i == garden->planterPickedUpIndex;

            entitiesToDraw[entitiesToDrawCount] = (Drawable){
                .type = DRAWABLE_PLANTER,
                .data = planter,
                .origin = isoTileOrigin,
                .tileDepth = zIndex,
                .highlight = i == tileHovered->planterIndex,
                .pickedUp = pickedUp,
            };

            entitiesToDrawCount++;

            int plantsCount = planter->plantGrid.tileCount;

            for (int j = 0; j < plantsCount; j++) {
                if (planter->plants[j].exists) {
                    Vector2 planterOrigin = garden_getTileOrigin(garden, planter->coords);

                    Vector2 plantOrigin
                        = planter_getPlantWorldPos(planter, &garden->transform, planterOrigin, j);

                    entitiesToDraw[entitiesToDrawCount] = (Drawable){
                        .type = DRAWABLE_PLANT,
                        .data = &planter->plants[j],
                        .origin = plantOrigin,
                        .tileDepth = zIndex,
                        .localDepth = getPlantZIndex(garden->transform.rotation, planter, j),
                        .highlight
                        = i == tileHovered->planterIndex && j == garden->planterTileHovered,
                        .pickedUp = pickedUp,
                    };

                    entitiesToDrawCount++;
                }
            }
        }
    }

    // Draw entities
    qsort(entitiesToDraw, entitiesToDrawCount, sizeof(Drawable), compareDrawableDepths);

    for (int i = 0; i < entitiesToDrawCount; i++) {
        Vector2 origin = entitiesToDraw[i].origin;
        Color color = entitiesToDraw[i].pickedUp ? (Color){255, 255, 255, 100} : WHITE;

        if (entitiesToDraw[i].type == DRAWABLE_PLANTER) {
            Planter *p = entitiesToDraw[i].data;

            planter_draw(p, origin, garden->transform.scale, garden->transform.rotation, color);

            if (entitiesToDraw[i].highlight) {
                BeginBlendMode(BLEND_ADDITIVE);
                planter_draw(p,
                    origin,
                    garden->transform.scale,
                    garden->transform.rotation,
                    (Color){255, 255, 255, 100});
                EndBlendMode();
            }
        } else {
            Plant *p = entitiesToDraw[i].data;

            plant_draw(p, origin, garden->transform.scale, color);

            if (entitiesToDraw[i].highlight) {
                BeginBlendMode(BLEND_ADDITIVE);
                plant_draw(p, origin, garden->transform.scale, (Color){255, 255, 255, 100});
                EndBlendMode();
            }
        }
    }

    // Draw available slots to put a plant when a plant cutting is selected
    if (toolSelected == GARDENING_TOOL_PLANT_CUTTING) {
        for (int i = 0; i < garden->tileGrid.tileCount; i++) {
            if (!garden->planters[i].exists) {
                continue;
            }

            Planter *planter = &garden->planters[i];

            for (int j = 0; j < planter->plantGrid.tileCount; j++) {
                // don't draw slot indicator if the planter has a plant in that slot
                if (planter->plants[j].exists
                    || (i == garden->tiles[garden->tileHovered].planterIndex
                        && j == garden->planterTileHovered)) {
                    continue;
                }

                Vector2 planterWorldPos = grid_coordsToWorldPoint(&garden->transform,
                    planter->coords.x,
                    planter->coords.y,
                    garden->tileGrid.tileWidth,
                    garden->tileGrid.tileHeight);

                Vector2 plantWorldPos
                    = planter_getPlantWorldPos(planter, &garden->transform, planterWorldPos, j);

                DrawEllipse(plantWorldPos.x, plantWorldPos.y, 10, 5, (Color){255, 255, 255, 225});
            }
        }
    }

    // Draw item on hovered tile
    if (garden->tileHovered != -1) {
        int hoveredIndex = garden->tileHovered;

        switch (toolSelected) {
        case GARDENING_TOOL_NONE:
            if (garden->planterPickedUpIndex != -1) {
                Planter *originalPlanter = &garden->planters[garden->planterPickedUpIndex];

                Vector2 drawOrigin = (Vector2){hoveredTile.left.x, hoveredTile.top.y};
                Planter p;
                Vector2 gridCoords
                    = grid_getCoordsFromTileIndex(garden->tileGrid.cols, hoveredIndex);

                Color color = {255, 255, 255, 200};

                planter_init(&p,
                    originalPlanter->type,
                    gridCoords,
                    garden->selectionRotation,
                    garden->tileGrid.tileWidth);

                planter_draw(
                    &p, drawOrigin, garden->transform.scale, garden->transform.rotation, color);

                for (int i = 0; i < p.plantGrid.tileCount; i++) {
                    Plant *plant = &originalPlanter->plants[i];
                    if (plant->exists) {

                        Vector2 plantOrigin
                            = planter_getPlantWorldPos(&p, &garden->transform, hoveredTile.left, i);

                        plant_draw(plant, plantOrigin, garden->transform.scale, color);
                    }
                }
            }

            break;
        case GARDENING_TOOL_PLANTER: {
            Vector2 drawOrigin = (Vector2){hoveredTile.left.x, hoveredTile.top.y};
            Planter p;
            Vector2 gridCoords = grid_getCoordsFromTileIndex(garden->tileGrid.cols, hoveredIndex);

            planter_init(&p,
                toolVariantSelected,
                gridCoords,
                garden->selectionRotation,
                garden->tileGrid.tileWidth);

            planter_draw(&p,
                drawOrigin,
                garden->transform.scale,
                garden->transform.rotation,
                (Color){255, 255, 255, 200});
        } break;

        case GARDENING_TOOL_PLANT_CUTTING: {
            int planterIndex = garden->tiles[hoveredIndex].planterIndex;
            Planter *planter = &garden->planters[planterIndex];

            Vector2 drawOrigin;

            if (planterIndex != -1 && planter->exists && planter->plantGrid.tileCount > 0) {
                Vector2 planterOrigin = garden_getTileOrigin(garden, planter->coords);

                drawOrigin = planter_getPlantWorldPos(
                    planter, &garden->transform, planterOrigin, garden->planterTileHovered);

            } else {
                float plantBasePosY = garden->tileGrid.tileHeight * garden->transform.scale / 2.0f;

                drawOrigin = (Vector2){
                    hoveredTile.bottom.x,
                    hoveredTile.bottom.y - plantBasePosY,
                };
            }

            Plant plant = {.health = 100};
            plant_init(&plant, toolVariantSelected);
            plant_draw(&plant, drawOrigin, garden->transform.scale, (Color){255, 255, 255, 200});
        } break;

        case GARDENING_TOOL_IRRIGATOR:
        case GARDENING_TOOL_NUTRIENTS:
        case GARDENING_TOOL_TRASH_BIN:
            break;

        case GARDENING_TOOL_COUNT:
            assert(false);
        }
    }

    // drawings for debug - maybe refactor into something to control this on runtime?

    bool showZIndexOnTile = false;
    bool showZIndexOnEntity = false;
    bool showPlanterIndexOnTile = true;
    bool drawTileBounds = false;
    bool drawPlantBounds = false;
    char buffer[8];

    if (drawPlantBounds) {
        for (int i = 0; i < garden->tileGrid.tileCount; i++) {
            Planter *planter = &garden->planters[i];

            if (planter->exists) {
                for (int j = 0; j < planter->plantGrid.tileCount; j++) {
                    if (planter->plants[j].exists) {

                        Vector2 plantCoords
                            = grid_getCoordsFromTileIndex(planter->plantGrid.cols, j);

                        Vector2 planterWorldPos = grid_coordsToWorldPoint(&garden->transform,
                            planter->coords.x,
                            planter->coords.y,
                            garden->tileGrid.tileWidth,
                            garden->tileGrid.tileHeight);

                        planterWorldPos.y -= planterDefinitions[planter->type].plantBasePosY;

                        SceneTransform localTransform = {
                            planterWorldPos,
                            garden->transform.rotation,
                            garden->transform.scale,
                        };

                        IsoRec isoRec = grid_toIsoRec(&localTransform,
                            plantCoords,
                            (Vector2){1, 1},
                            planter->plantGrid.tileWidth,
                            planter->plantGrid.tileHeight);

                        drawIsoRectangleLines(garden, isoRec, 1, RED);
                    }
                }
            }
        }
    }

    for (int i = 0; i < garden->tileGrid.tileCount; i++) {
        IsoRec currentTile = getTileIsoVertices(garden, i);

        if (drawTileBounds) {
            drawIsoRectangleLines(garden, currentTile, 1, (Color){255, 109, 194, 50});
        }

        if (showZIndexOnTile) {
            Vector2 tileCoords = grid_getCoordsFromTileIndex(garden->tileGrid.cols, i);

            int zIndex = grid_getZIndex(garden->transform.rotation,
                tileCoords.x,
                tileCoords.y,
                garden->tileGrid.cols,
                garden->tileGrid.rows);

            snprintf(buffer, 8, "%d", zIndex);

            DrawText(buffer,
                currentTile.right.x - ((currentTile.right.x - currentTile.left.x) / 2),
                currentTile.bottom.y - 20,
                12,
                WHITE);
        }

        if (showZIndexOnEntity) {
            Planter *planter = &garden->planters[i];

            if (planter->exists) {
                int planterTileIndex = grid_getTileIndexFromCoords(garden->tileGrid.cols,
                    garden->tileGrid.rows,
                    planter->coords.x,
                    planter->coords.y);

                int zIndex = getPlanterZIndex(garden, planterTileIndex);

                IsoRec planterTile = getTileIsoVertices(garden, planterTileIndex);

                Vector2 planterWorldPos = grid_coordsToWorldPoint(&garden->transform,
                    planter->coords.x,
                    planter->coords.y,
                    garden->tileGrid.tileWidth,
                    garden->tileGrid.tileHeight);

                snprintf(buffer, 8, "%d", zIndex);

                DrawText(buffer,
                    planterTile.bottom.x,
                    planterTile.bottom.y - (int)(garden->tileGrid.tileHeight / 2),
                    12,
                    PURPLE);

                for (int j = 0; j < planter->plantGrid.tileCount; j++) {
                    if (planter->plants[j].exists) {
                        Vector2 plantWorldPos = planter_getPlantWorldPos(
                            planter, &garden->transform, planterWorldPos, j);

                        int zIndex = getPlantZIndex(garden->transform.rotation, planter, j);

                        snprintf(buffer, 8, "%d", zIndex);

                        DrawText(buffer, plantWorldPos.x, plantWorldPos.y, 12, PURPLE);
                    }
                }
            }
        }

        if (showPlanterIndexOnTile) {
            int planterIndex = garden->tiles[i].planterIndex;

            if (planterIndex != -1) {

                snprintf(buffer, 8, "%d", planterIndex);

                DrawText(buffer,
                    currentTile.right.x - ((currentTile.right.x - currentTile.left.x) / 2),
                    currentTile.bottom.y - ((currentTile.bottom.y - currentTile.top.y) / 2),
                    20,
                    WHITE);
            }
        }
    }

    snprintf(buffer, 8, "GR %d", garden->transform.rotation);
    DrawText(buffer, 600, 0, 20, WHITE);
    snprintf(buffer, 8, "SR %d", garden->selectionRotation);
    DrawText(buffer, 600, 30, 20, WHITE);

    // DrawCircle(garden->lightSourcePos.x,
    //     garden->lightSourcePos.y,
    //     LIGHT_SOURCE_RADIUS * garden->transform.scale,
    //     YELLOW);
}
