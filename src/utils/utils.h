#include "../game/gameplay.h"
#include <raylib.h>

float utils_clampf(float min, float max, float value);
Rectangle utils_getRotatedRec(Rectangle rec, Rotation rotation);
Rotation utils_rotate(Rotation initialRotation, int steps);
