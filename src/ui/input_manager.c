#include "input_manager.h"
#include <raylib.h>
#include <stdio.h>

void inputManager_update(InputManager *input, float scale, Vector2 screenOffset) {
    Vector2 mousePositionAbsolute = GetMousePosition();

    Vector2 newWorldMousePos = (Vector2){
        (mousePositionAbsolute.x - screenOffset.x) / scale,
        (mousePositionAbsolute.y - screenOffset.y) / scale,
    };

    input->worldMouseDelta = (Vector2){
        input->worldMousePos.x - newWorldMousePos.x,
        input->worldMousePos.y - newWorldMousePos.y,
    };

    input->worldMousePos = newWorldMousePos;

    input->mouseButtonPressed = -1;
    for (int i = 0; i < 3; i++) {
        if (IsMouseButtonPressed(i)) {
            input->mouseButtonPressed = i;
            break;
        }
    }

    // If we need more than one key pressed here, change this
    input->keyPressed = GetKeyPressed();
}

// For debug purposes
void inputManager_drawMousePos(InputManager *input, Vector2 screenSize) {
    int fontSize = 30;

    char buffer[64];

    snprintf(buffer, 64, "world mouse: %f, %f", input->worldMousePos.x, input->worldMousePos.y);

    int textPosY = screenSize.y - (2 * fontSize);

    DrawText(buffer, 0, textPosY, fontSize, BLACK);

    textPosY += fontSize;

    Vector2 screenMouse = GetMousePosition();

    snprintf(buffer, 64, "screen mouse: %f, %f", screenMouse.x, screenMouse.y);

    DrawText(buffer, 0, textPosY, fontSize, BLACK);
}
