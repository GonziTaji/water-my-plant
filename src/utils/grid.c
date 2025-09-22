#include "grid.h"
#include <assert.h>

bool grid_isValidCoords(int gridCols, int gridRows, float x, float y) {
    if (x < 0 || y < 0 || x >= gridCols || y >= gridRows) {
        return false;
    }

    return true;
}

Vector2 grid_getCoordsFromTileIndex(int gridCols, int i) {
    // should avoid division by 0? Crashes here expose bugs,
    // and should never happen (getting grid coords from a grid with 0 cols?)
    return (Vector2){i % gridCols, (int)(i / gridCols)};
}

/** Returns -1 if the coords are not a valid cell of the grid */
int grid_getTileIndexFromCoords(int gridCols, int gridRows, float x, float y) {
    // get this out and use it when it's really necesary? to avoid gridRows as function parameter
    if (!grid_isValidCoords(gridCols, gridRows, x, y)) {
        return -1;
    }

    return ((int)y * gridCols) + (int)x;
}

Vector2 grid_worldPointToCoords(
    SceneTransform *transform, float x, float y, float tileWidth, float tileHeight) {

    const float TW = tileWidth * transform->scale;
    const float TH = tileHeight * transform->scale;

    const float dx = x - transform->translation.x;
    const float dy = y - transform->translation.y;

    Vector2 gridCoords = {};

    // TODO: refactor in some way? ugly
    switch ((int)transform->rotation) {
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

Vector2 grid_coordsToWorldPoint(
    const SceneTransform *transform, float x, float y, float tileWidth, float tileHeight) {

    int sumX = 0;
    int sumY = 0;

    // TODO: refactor in some way? ugly
    switch ((int)transform->rotation) {
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
        transform->translation.x + (sumX * tileWidth * transform->scale / 2.0f),
        transform->translation.y + (sumY * tileHeight * transform->scale / 2.0f),
    };
}

IsoRec grid_toIsoRec(const SceneTransform *transform,
    Vector2 coords,
    Vector2 size,
    float tileWidth,
    float tileHeight) {

    Vector2 left = {coords.x, coords.y};
    Vector2 top = {coords.x + size.x, coords.y};
    Vector2 right = {coords.x + size.x, coords.y + size.y};
    Vector2 bottom = {coords.x, coords.y + size.y};

    IsoRec isoRec = {
        grid_coordsToWorldPoint(transform, left.x, left.y, tileWidth, tileHeight),
        grid_coordsToWorldPoint(transform, top.x, top.y, tileWidth, tileHeight),
        grid_coordsToWorldPoint(transform, right.x, right.y, tileWidth, tileHeight),
        grid_coordsToWorldPoint(transform, bottom.x, bottom.y, tileWidth, tileHeight),
    };

    utils_rotateIsoRec(&isoRec, transform->rotation);

    return isoRec;
}
