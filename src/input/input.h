#pragma once

#include <raylib.h>

typedef struct {
    Vector2 worldMousePos;
    Vector2 worldMouseDelta;
    int mouseButtonPressed;
    int keyPressed;
} InputManager;

void inputManager_init(InputManager *input);
void input_update(InputManager *input, float scale, Vector2 screenOffset);
void input_drawMousePos(InputManager *input, Vector2 screenSize);
