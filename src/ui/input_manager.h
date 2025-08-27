#pragma once

#include <raylib.h>

typedef struct {
    Vector2 worldMousePos;
    Vector2 worldMouseDelta;
    bool mouseButtonPressed[3];
} InputManager;

void inputManager_update(InputManager *input, float scale, Vector2 screenOffset);
void inputManager_drawMousePos(InputManager *input, Vector2 screenSize);
