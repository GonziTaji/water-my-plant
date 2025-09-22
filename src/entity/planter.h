#pragma once

#include "../game/gameplay.h"
#include "../utils/grid.h"
#include "plant.h"
#include <raylib.h>

// 3x3 o 2x4 maximo por ahora
#define PLANTER_MAX_PLANTS 9

typedef enum {
    PLANTER_TYPE_NORMAL,
    PLANTER_TYPE_1x2,
    PLANTER_TYPE_2x2,
    // LOL, maybe rename planter to furniture?
    PLANTER_COUCH,
    PLANTER_TYPE_COUNT,
} PlanterType;

typedef struct {
    PlanterType type;
    bool exists;
    Plant plants[PLANTER_MAX_PLANTS];
    TileGrid plantGrid;
    /// coords and size in container grid
    Vector2 coords;
    Vector2 size;
    Rotation rotation;
    /// to align bottom of plant sprite to planter soil
    int plantBasePosY;
} Planter;

Vector2 planter_getFootPrint(PlanterType planterType, Rotation rotation);

int planter_getPlantCount(const Planter *planter);

void planter_empty(Planter *planter);

void planter_init(
    Planter *planter, PlanterType type, Vector2 coords, Rotation rotation, int tileWidth);

void planter_addPlant(Planter *planter, int index, enum PlantType type);

Rectangle planter_getSpriteSourceRec(
    PlanterType type, Rotation planterRotation, Rotation viewRotation);

void planter_draw(Planter *planter, Vector2 origin, float scale, Rotation rotation, Color color);

Vector2 planter_getPlantWorldPos(
    Planter *planter, SceneTransform *transform, Vector2 planterWorldPos, int plantIndex);

Vector2 planter_getPlantCoords(Planter *planter, int plantIndex);

int planter_getPlantIndexFromGridCoords(Planter *planter, Vector2 point);

int planter_getPlantIndexFromWorldPos(
    Planter *planter, SceneTransform *transform, Vector2 planterWorldPos, Vector2 point);
