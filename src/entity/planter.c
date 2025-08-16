
#include "planter.h"
#include <raylib.h>

void planter_init(Planter *planter, Rectangle bounds) {
    planter->bounds = bounds;
    planter->hasPlant = false;
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
