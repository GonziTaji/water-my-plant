#include "planter.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include <raylib.h>

Vector2 planter_getDimensions(PlanterType planterType) {
    switch (planterType) {
    case PLANTER_TYPE_NORMAL:
        return (Vector2){1, 1};

    case PLANTER_TYPE_2_X_2:
        return (Vector2){2, 3};

    case PLANTER_TYPE_COUNT:
        return (Vector2){1, 1};
    }

    return (Vector2){0, 0};
}

void planter_init(Planter *planter, PlanterType type, Vector2 origin) {
    planter->alive = true;
    planter->hasPlant = false;
    planter->origin = origin;
    planter->dimensions = planter_getDimensions(type);
    planter->plantBasePosY = 48;
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

void planter_draw(Planter *planter, Vector2 origin) {
    Rectangle source = {0, 0, planterTexture.width, planterTexture.height};

    Rectangle dest = {
        origin.x,
        origin.y,
        64 * WORLD_SCALE,
        64 * WORLD_SCALE,
    };

    Vector2 pivot = {dest.width / 2, dest.height};

    // TODO: sprite based on type
    DrawTexturePro(planterTexture, source, dest, pivot, 0, WHITE);
}
