#include "planter.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "../utils/utils.h"
#include <assert.h>
#include <raylib.h>

static const PlanterDefinition planterDefinitions[PLANTER_TYPE_COUNT] = {
    [PLANTER_TYPE_NORMAL] = {
        .size = {1, 1},
        .spriteExtraHeight = 0,
        .plantBasePosY = 12,
        .cols = 1,
        .rows = 1,
    },

    [PLANTER_TYPE_1x2] = {
        .size = {2, 4},
        .spriteExtraHeight = 0,
        .plantBasePosY = 18,
        .cols = 1,
        .rows = 2,
    },

    [PLANTER_TYPE_2x2] = {
        .size = {4, 4},
        .spriteExtraHeight = 0.5,
        .plantBasePosY = 18,
        .cols = 3,
        .rows = 3,
    },

    [PLANTER_COUCH] = {
        .size = {3, 4},
        .spriteExtraHeight = 1.5,
        .plantBasePosY = 0,
        .cols = 0,
        .rows = 0,
    },
};

static Vector3 getIsoDimensions(PlanterType planterType, Rotation rotation) {
    Vector2 size = planterDefinitions[planterType].size;
    float height = planterDefinitions[planterType].spriteExtraHeight;

    utils_rotateVector(&size, rotation);

    return (Vector3){
        (size.x + size.y) * TILE_WIDTH / 2,
        (size.x + size.y) * TILE_HEIGHT / 2,
        height * TILE_HEIGHT,
    };
}

static Vector2 getSpriteDimensions(PlanterType type, Rotation rotation) {
    Vector3 dimensions = getIsoDimensions(type, rotation);

    return (Vector2){
        dimensions.x,
        dimensions.y + dimensions.z,
    };
}

/// internal plant grid of the planter
static TileGrid getGrid(PlanterType planterType, Rotation rotation, int worldTileWidth) {
    Vector2 dimensions = planter_getFootPrint(planterType, rotation);

    TileGrid grid;

    grid.cols = planterDefinitions[planterType].cols;
    grid.rows = planterDefinitions[planterType].rows;

    Vector2 gd = (Vector2){grid.cols, grid.rows};
    utils_rotateVector(&gd, rotation);
    grid.cols = gd.x;
    grid.rows = gd.y;

    assert(dimensions.x >= grid.cols);
    assert(dimensions.y >= grid.rows);

    grid.tileCount = grid.cols * grid.rows;

    grid.tileHeight = (worldTileWidth / 2.0f) * (dimensions.x / grid.cols);
    grid.tileWidth = worldTileWidth * (dimensions.y / grid.rows);

    return grid;
}

/// cols and rows that the planter uses in the garden grid
Vector2 planter_getFootPrint(PlanterType planterType, Rotation rotation) {
    assert(planterType != PLANTER_TYPE_COUNT);

    Vector2 d = {
        planterDefinitions[planterType].size.x,
        planterDefinitions[planterType].size.y,
    };

    utils_rotateVector(&d, rotation);

    return d;
}

void planter_empty(Planter *planter) {
    planter->type = 0;
    planter->exists = false;
    planter->rotation = 0;
    planter->coords = (Vector2){0, 0};
}

void planter_init(
    Planter *planter, PlanterType type, Vector2 coords, Rotation rotation, int tileWidth) {
    planter->type = type;
    planter->exists = true;
    planter->rotation = rotation;
    planter->coords = coords;
    planter->plantGrid = getGrid(type, rotation, tileWidth);

    int plantCount = planter->plantGrid.tileCount;
    for (int i = 0; i < plantCount; i++) {
        planter->plants[i].exists = false;
    }
}

int planter_getPlantIndexFromGridCoords(Planter *planter, Vector2 coords) {
    int plantX = coords.x - planter->coords.x;
    int plantY = coords.y - planter->coords.y;

    const Vector2 *size = &planterDefinitions[planter->type].size;

    int plantIndex = grid_getTileIndexFromCoords(size->x, size->x, plantX, plantY);

    return plantIndex;
}

int planter_getPlantIndexFromWorldPos(
    Planter *planter, SceneTransform *transform, Vector2 planterWorldPos, Vector2 point) {

    SceneTransform localTransform = {
        planterWorldPos,
        transform->rotation,
        transform->scale,
    };

    Vector2 plantCoords = grid_worldPointToCoords(&localTransform,
        point.x,
        point.y,
        planter->plantGrid.tileWidth,
        planter->plantGrid.tileHeight);

    return grid_getTileIndexFromCoords(
        planter->plantGrid.cols, planter->plantGrid.rows, plantCoords.x, plantCoords.y);
}

Vector2 planter_getPlantWorldPos(
    Planter *planter, SceneTransform *transform, Vector2 planterWorldPos, int plantIndex) {

    Vector2 plantCoords = grid_getCoordsFromTileIndex(planter->plantGrid.cols, plantIndex);

    SceneTransform localTransform = {
        planterWorldPos,
        transform->rotation,
        transform->scale,
    };

    IsoRec isoRec = grid_toIsoRec(&localTransform,
        plantCoords,
        (Vector2){1, 1},
        planter->plantGrid.tileWidth,
        planter->plantGrid.tileHeight);

    isoRec.bottom.y -= (planterDefinitions[planter->type].plantBasePosY * transform->scale);

    return isoRec.bottom;
}

void planter_addPlant(Planter *planter, int index, enum PlantType type) {
    if (planter->plants[index].exists) {
        return;
    }

    plant_init(&planter->plants[index], type);
}

Rectangle planter_getSpriteSourceRec(
    PlanterType type, Rotation planterRotation, Rotation viewRotation) {

    Rotation finalRotation = utils_rotate(planterRotation, viewRotation);
    Vector2 spriteDimensions = getSpriteDimensions(type, finalRotation);

    float planterOriginY = 0;

    for (int i = 0; i < PLANTER_TYPE_COUNT; i++) {
        if (i == type) {
            break;
        }

        Vector2 d = getSpriteDimensions(i, finalRotation);

        planterOriginY += d.y;
    }

    Vector2 spriteOrigin = (Vector2){
        finalRotation * spriteDimensions.x,
        planterOriginY,
    };

    Rectangle source = {
        spriteOrigin.x,
        spriteOrigin.y,
        spriteDimensions.x,
        spriteDimensions.y,
    };

    return source;
}

void planter_draw(
    Planter *planter, Vector2 origin, float scale, Rotation viewRotation, Color color) {

    Rectangle source = planter_getSpriteSourceRec(planter->type, planter->rotation, viewRotation);

    Rectangle dest = {
        origin.x,
        origin.y,
        source.width * scale,
        source.height * scale,
    };

    Vector3 isoDimensions
        = getIsoDimensions(planter->type, utils_rotate(planter->rotation, viewRotation));

    Vector2 pivot = {0, isoDimensions.z * scale};

    DrawTexturePro(planterAtlas, source, dest, pivot, 0, color);
}
