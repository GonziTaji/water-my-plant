#pragma once

#include "plant.h"
#include <raylib.h>

typedef struct {
    bool hasPlant;
    Plant plant;
    Rectangle bounds;
} Planter;

void planter_init(Planter *planter, Rectangle bounds);
void planter_addPlant(Planter *planter);
void planter_removePlant(Planter *planter);
