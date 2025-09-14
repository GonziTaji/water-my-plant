#pragma once

#include "../game/gameplay.h"
#include <raylib.h>

// WIP
typedef struct {
    int cols;
    int rows;
    int tileWidth;
    int tileHeight;
    int tileCount;
} TileGrid;

typedef struct {
    Vector2 translation;
    Rotation rotation;
    float scale;
} SceneTransform;

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

// rec and isometric transform utils
Rectangle utils_getRotatedRec(Rectangle rec, Rotation rotation);
Rotation utils_rotate(Rotation initialRotation, int steps);
IsoRec utils_toIsoRec(
    const SceneTransform *transform, Rectangle rec, float tileWidth, float tileHeight);
void utils_rotateIsoRec(IsoRec *isoRec, Rotation rotation);

// grid
bool utils_grid_isValidCoords(int gridCols, int gridRows, float x, float y);
Vector2 utils_grid_getCoordsFromTileIndex(int gridCols, int i);
int utils_grid_getTileIndexFromCoords(int gridCols, int gridRows, float x, float y);
Vector2 utils_grid_worldPointToCoords(
    SceneTransform *transform, float x, float y, float tileWidth, float tileHeight);
Vector2 utils_grid_coordsToWorldPoint(
    const SceneTransform *transform, float x, float y, float tileWidth, float tileHeight);
