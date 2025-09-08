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

void planter_init(Planter *planter, PlanterType type, Vector2 origin, Rotation rotation) {
    planter->type = type;
    planter->alive = true;
    planter->hasPlant = false;
    planter->rotation = rotation;
    // TODO: based on type
    planter->plantBasePosY = 24;

    Vector2 dimensions = planter_getDimensions(type, rotation);

    planter->bounds = (Rectangle){
        origin.x,
        origin.y,
        dimensions.x,
        dimensions.y,
    };
}

void planter_addPlant(Planter *planter, enum PlantType type) {
    if (planter->hasPlant) {
        return;
    }

    plant_init(&planter->plant, type);
    planter->hasPlant = true;
}

void planter_removePlant(Planter *planter) {
    planter->hasPlant = false;
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
