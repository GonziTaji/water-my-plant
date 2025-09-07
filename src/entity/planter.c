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

    case PLANTER_TYPE_2_X_3:
        d = (Vector2){2, 3};
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
    planter->plantBasePosY = 48;

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

void planter_draw(Planter *planter, Vector2 origin, float scale) {
    Rectangle source = {0, 0, planterTexture.width, planterTexture.height};

    Rectangle dest = {
        origin.x,
        origin.y,
        64 * scale,
        64 * scale,
    };

    Vector2 pivot = {dest.width / 2, dest.height};

    // TODO: sprite based on type
    DrawTexturePro(planterTexture, source, dest, pivot, 0, WHITE);
}
