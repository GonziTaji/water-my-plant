
#include "planter.h"
#include "../game/constants.h"
#include <raylib.h>

Texture2D planterTexture;

void planter_loadTextures() {
    planterTexture = LoadTexture("resources/assets/planter-1.png");
}

void planter_unloadTextures() {
    UnloadTexture(planterTexture);
}

void planter_init(Planter *planter) {
    planter->hasPlant = false;
    // Depende del tipo de planter. TBD
    planter->height = 48;
}

void planter_addPlant(Planter *planter) {
    if (planter->hasPlant) {
        return;
    }

    plant_init(&planter->plant);
    planter->hasPlant = true;
}

void planter_removePlant(Planter *planter) {
    planter->hasPlant = false;
}

void planter_draw(Planter *planter, Vector2 origin) {
    // TODO: sprite dimensions
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
