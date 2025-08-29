#pragma once

#include "plant.h"
#include <raylib.h>

typedef struct {
    bool hasPlant;
    Plant plant;
    // Depende del tipo de planter, usado para renderizar la planta. TBD
    int height;
} Planter;

void planter_loadTextures();
void planter_init(Planter *planter);
void planter_addPlant(Planter *planter, enum PlantType type);
void planter_removePlant(Planter *planter);
void planter_draw(Planter *planter, Vector2 origin);
