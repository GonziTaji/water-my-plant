#pragma once

#include "utils.h"
#include <raylib.h>
#include <stdbool.h>

// WIP
typedef struct {
    int cols;
    int rows;
    int tileWidth;
    int tileHeight;
    int tileCount;
} TileGrid;

bool grid_isValidCoords(int gridCols, int gridRows, float x, float y);

Vector2 grid_getCoordsFromTileIndex(int gridCols, int i);

int grid_getTileIndexFromCoords(int gridCols, int gridRows, int x, int y);

Vector2 grid_worldPointToCoords(
    IsoTransform *transform, float x, float y, float tileWidth, float tileHeight);

Vector2 grid_coordsToWorldPoint(
    const IsoTransform *transform, float x, float y, float tileWidth, float tileHeight);

IsoRec grid_toIsoRec(
    const IsoTransform *transform, Vector2 coords, Vector2 size, float tileWidth, float tileHeight);

Vector2 grid_getDistanceFromFarthestTile(Rotation rotation, int x, int y, int cols, int rows);

int grid_getZIndex(Rotation rotation, int x, int y, int cols, int rows);

Vector2 grid_getTileOrigin(IsoTransform *transform, Vector2 coords, int tileWidth, int tileHeight);

Vector2 grid_rotateCoords(Vector2 coords, Rotation rotation, int cols, int rows);
