#include "planter.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "../utils/utils.h"
#include <raylib.h>

Vector2 planter_getDimensions(PlanterType planterType, Rotation rotation) {
    Vector2 d = (Vector2){0, 0};

    switch (planterType) {
    case PLANTER_TYPE_NORMAL:
        d = (Vector2){1, 1};
        break;

    case PLANTER_TYPE_1x2:
        d = (Vector2){1, 2};
        break;

    case PLANTER_TYPE_2x2:
        d = (Vector2){2, 2};
        break;

    case PLANTER_TYPE_COUNT:
        d = (Vector2){1, 1};
        break;
    }

    if (rotation == ROTATION_90 || rotation == ROTATION_270) {
        float aux = d.x;
        d.x = d.y;
        d.y = aux;
    }

    return d;
}

int planter_getPlantCount(const Planter *planter) {
    return planter->bounds.height * planter->bounds.width;
}

void planter_empty(Planter *planter) {
    planter->type = 0;
    planter->exists = false;
    planter->rotation = 0;
    planter->plantBasePosY = 0;
    planter->bounds = (Rectangle){0, 0, 0, 0};
}

void planter_init(Planter *planter, PlanterType type, Vector2 origin, Rotation rotation) {
    planter->type = type;
    planter->exists = true;
    planter->rotation = rotation;

    Vector2 dimensions = planter_getDimensions(type, rotation);

    planter->bounds = (Rectangle){
        origin.x,
        origin.y,
        dimensions.x,
        dimensions.y,
    };

    const int plantsCount = planter_getPlantCount(planter);

    for (int i = 0; i < plantsCount; i++) {
        planter->plants[i].exists = false;
    }

    switch (type) {
    case PLANTER_TYPE_NORMAL:
        planter->plantBasePosY = 24;
        break;
    case PLANTER_TYPE_1x2:
        planter->plantBasePosY = 18;
        break;
    case PLANTER_TYPE_2x2:
        planter->plantBasePosY = 18;
        break;
    case PLANTER_TYPE_COUNT:
        break;
    }
}

int planter_getPlantIndexFromGridCoords(Planter *planter, Vector2 coords) {
    int plantX = coords.x - planter->bounds.x;
    int plantY = coords.y - planter->bounds.y;

    int plantIndex = utils_grid_getTileIndexFromCoords(
        planter->bounds.width, planter->bounds.height, plantX, plantY);

    return plantIndex;
}

Vector2 planter_getPlantCoords(Planter *planter, SceneTransform *transform, int plantIndex) {
    Rotation planterRotation = utils_rotate(planter->rotation, transform->rotation);
    Rectangle rotatedPlanterRec = utils_getRotatedRec(planter->bounds, planterRotation);

    Vector2 coords = utils_grid_getCoordsFromTileIndex(planter->bounds.width, plantIndex);
    coords.x += rotatedPlanterRec.x;
    coords.y += rotatedPlanterRec.y;

    return coords;
}

Vector2 planter_getPlantWorldPos(Planter *planter, SceneTransform *transform, int plantIndex) {
    Vector2 plantCoords = planter_getPlantCoords(planter, transform, plantIndex);
    Rectangle plantBounds = (Rectangle){plantCoords.x, plantCoords.y, 1, 1};

    IsoRec isoRec = utils_toIsoRec(transform, plantBounds);

    return (Vector2){
        isoRec.bottom.x,
        isoRec.bottom.y - (planter->plantBasePosY * transform->scale),
    };
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
    Vector2 planterDimensions = planter_getDimensions(type, finalRotation);

    Vector2 spriteDimensions = (Vector2){
        (planterDimensions.x + planterDimensions.y) * TILE_WIDTH / 2,
        (planterDimensions.x + planterDimensions.y) * TILE_HEIGHT / 2,
    };

    float planterOriginY = 0;

    for (int i = 0; i < PLANTER_TYPE_COUNT; i++) {
        if (i == type) {
            break;
        }

        Vector2 d = planter_getDimensions(i, viewRotation);

        planterOriginY += (d.x + d.y) * TILE_HEIGHT / 2;
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

    Vector2 pivot = {0, 0};

    // TODO: sprite based on type
    DrawTexturePro(planterAtlas, source, dest, pivot, 0, color);
    // DrawRectangleLinesEx(dest, 2, BLACK);
}
