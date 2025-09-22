#include "planter.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "../utils/utils.h"
#include <assert.h>
#include <raylib.h>

static Vector3 getIsoDimensions(PlanterType planterType, Rotation rotation) {
    Vector2 f = planter_getFootPrint(planterType, rotation);

    Vector3 dimensionsOnGrid = {f.x, f.y, 0};

    if (planterType == PLANTER_COUCH) {
        dimensionsOnGrid.z = 1.5;
    } else if (planterType == PLANTER_TYPE_2x2) {
        dimensionsOnGrid.z = 0.5;
    }

    return (Vector3){
        (dimensionsOnGrid.x + dimensionsOnGrid.y) * TILE_WIDTH / 2,
        (dimensionsOnGrid.x + dimensionsOnGrid.y) * TILE_HEIGHT / 2,
        dimensionsOnGrid.z * TILE_HEIGHT,
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
static TileGrid getGrid(PlanterType planterType, Rotation rotation, int worldTileGrid) {
    Vector2 dimensions = planter_getFootPrint(planterType, rotation);

    TileGrid grid;

    switch (planterType) {
    case PLANTER_TYPE_NORMAL:
        grid.cols = 1;
        grid.rows = 1;
        break;

    case PLANTER_TYPE_1x2:
        grid.cols = 1;
        grid.rows = 2;
        break;

    case PLANTER_TYPE_2x2:
        grid.cols = 3;
        grid.rows = 3;
        break;

    case PLANTER_COUCH:
        grid.cols = 0;
        grid.rows = 0;
        break;

    case PLANTER_TYPE_COUNT:
        assert(false);
        break;
    }

    Vector2 gd = (Vector2){grid.cols, grid.rows};
    utils_rotateVector(&gd, rotation);
    grid.cols = gd.x;
    grid.rows = gd.y;

    assert(dimensions.x >= grid.cols);
    assert(dimensions.y >= grid.rows);

    grid.tileCount = grid.cols * grid.rows;

    grid.tileHeight = (worldTileGrid / 2.0f) * (dimensions.x / grid.cols);
    grid.tileWidth = worldTileGrid * (dimensions.y / grid.rows);

    return grid;
}

/// cols and rows that the planter uses in the garden grid
Vector2 planter_getFootPrint(PlanterType planterType, Rotation rotation) {
    Vector2 d = (Vector2){0, 0};

    switch (planterType) {
    case PLANTER_TYPE_NORMAL:
        d = (Vector2){1, 1};
        break;

    case PLANTER_TYPE_1x2:
        d = (Vector2){2, 4};
        break;

    case PLANTER_TYPE_2x2:
        d = (Vector2){4, 4};
        break;

    case PLANTER_COUCH:
        d = (Vector2){3, 4};
        break;

    case PLANTER_TYPE_COUNT:
        d = (Vector2){1, 1};
        break;
    }

    utils_rotateVector(&d, rotation);

    return d;
}

// remove? there's value in an interface in case the plant count has logic involved
int planter_getPlantCount(const Planter *planter) {
    return planter->plantGrid.tileCount;
}

void planter_empty(Planter *planter) {
    planter->type = 0;
    planter->exists = false;
    planter->rotation = 0;
    planter->plantBasePosY = 0;
    planter->size = (Vector2){0, 0};
    planter->coords = (Vector2){0, 0};
}

void planter_init(
    Planter *planter, PlanterType type, Vector2 coords, Rotation rotation, int tileWidth) {
    planter->type = type;
    planter->exists = true;
    planter->rotation = rotation;
    planter->size = planter_getFootPrint(type, rotation);
    planter->coords = coords;
    planter->plantGrid = getGrid(type, rotation, tileWidth);

    for (int i = 0; i < planter->plantGrid.tileCount; i++) {
        planter->plants[i].exists = false;
    }

    switch (type) {
    case PLANTER_TYPE_NORMAL:
        planter->plantBasePosY = 12;
        break;
    case PLANTER_TYPE_1x2:
        planter->plantBasePosY = 18;
        break;
    case PLANTER_TYPE_2x2:
        planter->plantBasePosY = 18;
        break;

    case PLANTER_COUCH:
        planter->plantBasePosY = 0;
        break;

    case PLANTER_TYPE_COUNT:
        break;
    }
}

int planter_getPlantIndexFromGridCoords(Planter *planter, Vector2 coords) {
    int plantX = coords.x - planter->coords.x;
    int plantY = coords.y - planter->coords.y;

    int plantIndex = grid_getTileIndexFromCoords(planter->size.x, planter->size.y, plantX, plantY);

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

    isoRec.bottom.y -= (planter->plantBasePosY * transform->scale);

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
