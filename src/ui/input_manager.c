#include "input_manager.h"
#include <raylib.h>

void inputManager_update(InputManager *input, float scale, Vector2 screenOffset) {
    Vector2 mousePositionAbsolute = GetMousePosition();

    input->mousePosVirtual.x = (mousePositionAbsolute.x - screenOffset.x) / scale;
    input->mousePosVirtual.y = (mousePositionAbsolute.y - screenOffset.y) / scale;

    for (int i = 0; i < 3; i++) {
        input->mouseButtonPressed[i] = IsMouseButtonPressed(i);
    }
}
