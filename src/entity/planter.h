#pragma once

#include "../game/gameplay.h"
#include "plant.h"
#include <raylib.h>

// 3x3 o 2x4 maximo por ahora
#define PLANTER_MAX_PLANTS 9

typedef enum {
    PLANTER_TYPE_NORMAL,
    PLANTER_TYPE_1x2,
    PLANTER_TYPE_2x2,
    PLANTER_TYPE_COUNT,
} PlanterType;

typedef struct {
    PlanterType type;
    bool exists;
    Plant plants[PLANTER_MAX_PLANTS];
    Rectangle bounds;
    Rotation rotation;
    /// to align bottom of plant sprite to planter soil
    int plantBasePosY;
} Planter;

Vector2 planter_getDimensions(PlanterType planterType, Rotation rotation);
int planter_getPlantCount(const Planter *planter);
void planter_empty(Planter *planter);
void planter_init(Planter *planter, PlanterType type, Vector2 origin, Rotation rotation);
void planter_addPlant(Planter *planter, int index, enum PlantType type);
Rectangle planter_getSpriteSourceRec(
    PlanterType type, Rotation planterRotation, Rotation viewRotation);
void planter_draw(Planter *planter, Vector2 origin, float scale, Rotation rotation, Color color);
Vector2 planter_getPlantWorldPos(Planter *planter, Camera2D *camera, int plantIndex);
Vector2 planter_getPlantCoords(Planter *planter, Camera2D *camera, int plantIndex);
int planter_getPlantIndexFromGridCoords(Planter *planter, Vector2 point);
