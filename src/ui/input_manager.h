#pragma once

#include <raylib.h>

typedef struct {
    Vector2 worldMousePos;
    Vector2 worldMouseDelta;
    bool mouseButtonPressed[3];
    // Keycodes de teclas presionadas aqui cuando llegue el punto en que varios
    // subsistemas necesiten chequear sobre un key presionado
} InputManager;

void inputManager_update(InputManager *input, float scale, Vector2 screenOffset);
void inputManager_drawMousePos(InputManager *input, Vector2 screenSize);
