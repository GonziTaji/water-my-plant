#include "utils.h"
#include <raylib.h>

float utils_clampf(float min, float max, float value) {
    if (min > value) {
        return min;
    }

    if (max < value) {
        return max;
    }

    return value;
}

Rectangle utils_getRotatedRec(Rectangle rec, Rotation rotation) {
    if (rec.height == 1 && rec.width == 1) {
        return rec;
    }

    switch (rotation) {
    case ROTATION_90:
    case ROTATION_270:
        return (Rectangle){rec.x, rec.y, rec.height, rec.width};

    case ROTATION_0:
    case ROTATION_180:
    default:
        return rec;
    }
}

Rotation utils_rotate(Rotation initialRotation, int steps) {
    Rotation steppedRotation = initialRotation + steps;
    return steppedRotation % ROTATION_COUNT;
}
