#include "input.h"
#include <raylib.h>
#include <stdio.h>

// maybe separate this funciton in steps like input_pollKeyboard, input_pollMouse and let the game
// decide the order of execution?
void input_update(InputManager *input, float scale, Vector2 screenOffset) {
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
void input_drawMousePos(InputManager *input, Vector2 screenSize) {
    int fontSize = 30;

    char buffer[64];

    snprintf(buffer, 64, "world mouse: %f, %f", input->worldMousePos.x, input->worldMousePos.y);

    int textPosY = screenSize.y - (2 * fontSize);

    DrawText(buffer, 0, textPosY, fontSize, WHITE);

    textPosY += fontSize;

    Vector2 screenMouse = GetMousePosition();

    snprintf(buffer, 64, "screen mouse: %f, %f", screenMouse.x, screenMouse.y);

    DrawText(buffer, 0, textPosY, fontSize, WHITE);
}
