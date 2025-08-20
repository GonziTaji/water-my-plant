#pragma once

#include <raylib.h>

typedef struct {
    Vector2 worldMousePos;
    bool mouseButtonPressed[3];
    // Keycodes de teclas presionadas aqui cuando llegue el punto en que varios
    // subsistemas necesiten chequear sobre un key presionado
} InputManager;

void inputManager_update(InputManager *input, float scale, Vector2 screenOffset);
