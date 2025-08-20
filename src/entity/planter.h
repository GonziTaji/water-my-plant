#pragma once

#include "plant.h"
#include <raylib.h>

typedef struct {
    bool hasPlant;
    Plant plant;
} Planter;

void planter_init(Planter *planter);
void planter_addPlant(Planter *planter);
void planter_removePlant(Planter *planter);
