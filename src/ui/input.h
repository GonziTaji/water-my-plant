#pragma once

#include <raylib.h>

typedef struct {
    Vector2 mousePosVirtual;
    // Keycodes de teclas presionadas aqui cuando llegue el punto en que varios
    // subsistemas necesiten chequear sobre un key presionado
} Input;

void input_update(Input *input, float scale, Vector2 screenOffset);
