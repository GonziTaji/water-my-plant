
#include "planter.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include <raylib.h>

void planter_init(Planter *planter) {
    planter->hasPlant = false;
    // Depende del tipo de planter. TBD
    planter->height = 48;
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

    DrawTexturePro(planterTexture, source, dest, pivot, 0, WHITE);
}
