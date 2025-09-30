#pragma once

#include "../game/gameplay.h"
#include <raylib.h>

typedef struct {
    Vector2 translation;
    Rotation rotation;
    float scale;
} IsoTransform;

typedef struct {
    Vector2 left;
    Vector2 top;
    Vector2 right;
    Vector2 bottom;
} IsoRec;

// misc
void utils_rotateVector(Vector2 *vector, Rotation rotation);

// math
float utils_clampf(float min, float max, float value);
float utils_absf(float f);

// rec and isometric transform utils
Rectangle utils_getRotatedRec(Rectangle rec, Rotation rotation);
Rotation utils_rotate(Rotation initialRotation, int steps);
void utils_rotateIsoRec(IsoRec *isoRec, Rotation rotation);
