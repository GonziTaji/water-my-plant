#include "input.h"
#include <raylib.h>

void input_update(Input *input, float scale, Vector2 screenOffset) {
    Vector2 mousePositionAbsolute = GetMousePosition();

    input->mousePosVirtual.x = (mousePositionAbsolute.x - screenOffset.x) / scale;
    input->mousePosVirtual.y = (mousePositionAbsolute.y - screenOffset.y) / scale;
}
