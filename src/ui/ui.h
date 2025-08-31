#pragma once
#include "../entity/garden.h"
#include "../game/gameplay.h"
#include "input_manager.h"
#include "ui_button_grid.h"
#include <raylib.h>

typedef struct Game Game;

typedef struct {
    UIButtonGrid toolSelectionButtonPannel;
    UIButtonGrid plantSelectionButtonPannel;
} UI;

void ui_init(UI *ui, Vector2 *screenSize);
Command ui_processInput(UI *ui, InputManager *input, enum GardeningTool tool);
void ui_draw(UI *ui,
    const InputManager *input,
    const Vector2 *screenSize,
    const Garden *garden,
    enum GardeningTool toolSelected);
