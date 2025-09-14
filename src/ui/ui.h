#pragma once
#include "../entity/garden.h"
#include "../game/gameplay.h"
#include "../input/input.h"
#include "ui_button_grid.h"
#include <raylib.h>
#include <stdbool.h>

typedef struct Game Game;

typedef struct {
    UIButtonGrid toolSelectionButtonPannel;
    UIButtonGrid toolVariantButtonPannel;
    UIButtonGrid speedSelectionButtonPannel;
    bool showToolVariantPanel;
} UI;

void ui_syncToolVariantPanelToSelection(
    UI *ui, enum GardeningTool toolSelected, int toolVariantSelected);

void ui_init(UI *ui, Vector2 *screenSize, GameplaySpeed gameplaySpeed);

Message ui_processInput(UI *ui, InputManager *input, enum GardeningTool tool);

void ui_draw(UI *ui,
    InputManager *input,
    Vector2 *screenSize,
    Garden *garden,
    enum GardeningTool toolSelected,
    float gameplayTime);
