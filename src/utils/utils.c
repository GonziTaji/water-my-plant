#include "utils.h"
#include "../game/constants.h"
#include <assert.h>
#include <raylib.h>

float utils_absf(float f) {
    return f > 0 ? f : -f;
}

void utils_rotateVector(Vector2 *vector, Rotation rotation) {
    if (rotation == ROTATION_90 || rotation == ROTATION_270) {
        float aux = vector->x;
        vector->x = vector->y;
        vector->y = aux;
    }
}

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
    if (rec.height == rec.width) {
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

void utils_rotateIsoRec(IsoRec *isoRec, Rotation rotation) {
    Vector2 source[4] = {
        isoRec->left,
        isoRec->top,
        isoRec->right,
        isoRec->bottom,
    };

    int rotatedIndex;
    for (int i = 0; i < ROTATION_COUNT; i++) {
        rotatedIndex = i + rotation;

        if (rotatedIndex >= ROTATION_COUNT) {
            rotatedIndex -= 4;
        }

        switch (rotatedIndex) {
        case 0:
            isoRec->left = source[i];
            break;
        case 1:
            isoRec->top = source[i];
            break;
        case 2:
            isoRec->right = source[i];
            break;
        case 3:
            isoRec->bottom = source[i];
            break;
        }
    }
}
