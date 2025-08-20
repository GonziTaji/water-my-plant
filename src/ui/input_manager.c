#include "input_manager.h"
#include <raylib.h>
#include <stdio.h>

void inputManager_update(InputManager *input, float scale, Vector2 screenOffset) {
    Vector2 mousePositionAbsolute = GetMousePosition();

    input->worldMousePos.x = (mousePositionAbsolute.x - screenOffset.x) / scale;
    input->worldMousePos.y = (mousePositionAbsolute.y - screenOffset.y) / scale;

    for (int i = 0; i < 3; i++) {
        input->mouseButtonPressed[i] = IsMouseButtonPressed(i);
    }

    if (input->mouseButtonPressed[0]) {
        printf("Mouse pressed at %f %f\n", input->worldMousePos.x, input->worldMousePos.y);
    }
}
