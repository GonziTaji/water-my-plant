#pragma once

#include "../game/gameplay.h"
#include "plant.h"
#include <raylib.h>

typedef enum {
    PLANTER_TYPE_NORMAL,
    PLANTER_TYPE_2_X_3,
    PLANTER_TYPE_COUNT,
} PlanterType;

typedef struct {
    PlanterType type;
    bool alive;
    bool hasPlant;
    Plant plant;
    Rectangle bounds;
    Rotation rotation;
    /// to align bottom of plant sprite to planter soil
    int plantBasePosY;
} Planter;

Vector2 planter_getDimensions(PlanterType planterType, Rotation rotation);
void planter_init(Planter *planter, PlanterType type, Vector2 origin, Rotation rotation);
void planter_addPlant(Planter *planter, enum PlantType type);
void planter_removePlant(Planter *planter);
void planter_draw(Planter *planter, Vector2 origin, float scale);
