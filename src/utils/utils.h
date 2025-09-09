#include "../game/gameplay.h"
#include <raylib.h>

typedef struct {
    Vector2 left;
    Vector2 top;
    Vector2 right;
    Vector2 bottom;
} IsoRec;

// math
float utils_clampf(float min, float max, float value);

// rec and isometric transform utils
Rectangle utils_getRotatedRec(Rectangle rec, Rotation rotation);
Rotation utils_rotate(Rotation initialRotation, int steps);
IsoRec utils_toIsoRec(const Camera2D *camera, Rectangle rec);
void utils_rotateIsoRec(IsoRec *isoRec, Rotation rotation);

// grid
Vector2 utils_grid_coordsToWorldPoint(const Camera2D *camera, float x, float y);
bool utils_grid_isValidCoords(int gridCols, int gridRows, float x, float y);
Vector2 utils_grid_getCoordsFromTileIndex(int gridCols, int i);
int utils_grid_getTileIndexFromCoords(int gridCols, int gridRows, float x, float y);
Vector2 utils_grid_worldPointToCoords(Camera2D *camera, float x, float y);
Vector2 utils_grid_coordsToWorldPoint(const Camera2D *camera, float x, float y);
IsoRec utils_toIsoRec(const Camera2D *camera, Rectangle rec);
