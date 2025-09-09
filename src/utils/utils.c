#include "utils.h"
#include "../game/constants.h"
#include <assert.h>
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

// grid section, move to a grid module?

bool utils_grid_isValidCoords(int gridCols, int gridRows, float x, float y) {
    if (x < 0 || y < 0 || x >= gridCols || y >= gridRows) {
        return false;
    }

    return true;
}

Vector2 utils_grid_getCoordsFromTileIndex(int gridCols, int i) {
    return (Vector2){i % gridCols, (int)(i / gridCols)};
}

/** Returns -1 if the coords are not a valid cell of the grid */
int utils_grid_getTileIndexFromCoords(int gridCols, int gridRows, float x, float y) {
    // get this out and use it when it's really necesary? to avoid gridRows as function parameter
    if (!utils_grid_isValidCoords(gridCols, gridRows, x, y)) {
        return -1;
    }

    return ((int)y * gridCols) + (int)x;
}

Vector2 utils_grid_worldPointToCoords(Camera2D *camera, float x, float y) {
    const float TW = TILE_WIDTH * camera->zoom;
    const float TH = TILE_HEIGHT * camera->zoom;

    const float dx = x - camera->offset.x;
    const float dy = y - camera->offset.y;

    Vector2 gridCoords = {};

    // TODO: refactor in some way? ugly
    switch ((int)camera->rotation) {
    case ROTATION_0:
        gridCoords.x = +(dx / TW) - (dy / TH);
        gridCoords.y = +(dx / TW) + (dy / TH);
        break;
    case ROTATION_90:
        gridCoords.x = +(dx / TW) + (dy / TH);
        gridCoords.y = -(dx / TW) + (dy / TH);
        break;
    case ROTATION_180:
        gridCoords.x = -(dx / TW) + (dy / TH);
        gridCoords.y = -(dx / TW) - (dy / TH);
        break;
    case ROTATION_270:
        gridCoords.x = -(dx / TW) - (dy / TH);
        gridCoords.y = +(dx / TW) - (dy / TH);
        break;
    case ROTATION_COUNT:
        // Handle in some way? give rotation initial vector?
        assert(false);
        break;
    }

    return gridCoords;
}

Vector2 utils_grid_coordsToWorldPoint(const Camera2D *camera, float x, float y) {
    int sumX = 0;
    int sumY = 0;

    // TODO: refactor in some way? ugly
    switch ((int)camera->rotation) {
    case ROTATION_0:
        sumX = +x + y;
        sumY = -x + y;
        break;
    case ROTATION_90:
        sumX = +x - y;
        sumY = +x + y;
        break;
    case ROTATION_180:
        sumX = -x - y;
        sumY = +x - y;
        break;
    case ROTATION_270:
        sumX = -x + y;
        sumY = -x - y;
        break;
    case ROTATION_COUNT:
        // Handle in some way? give rotation initial vector?
        break;
    }

    return (Vector2){
        camera->offset.x + (sumX * TILE_WIDTH * camera->zoom / 2.0f),
        camera->offset.y + (sumY * TILE_HEIGHT * camera->zoom / 2.0f),
    };
}

IsoRec utils_toIsoRec(const Camera2D *camera, Rectangle rec) {
    IsoRec isoRec = {
        utils_grid_coordsToWorldPoint(camera, rec.x, rec.y),
        utils_grid_coordsToWorldPoint(camera, rec.x + rec.width, rec.y),
        utils_grid_coordsToWorldPoint(camera, rec.x + rec.width, rec.y + rec.height),
        utils_grid_coordsToWorldPoint(camera, rec.x, rec.y + rec.height),
    };

    return isoRec;
}
