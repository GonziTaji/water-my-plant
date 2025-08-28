#pragma once
#include "../entity/garden.h"
#include "../entity/planter.h"
#include "../game/gameplay.h"
#include "button_pannel.h"
#include "input_manager.h"
#include <raylib.h>

typedef struct Game Game;

typedef struct {
    ButtonPannel mainButtonPannel;
    ButtonPannel plantSelectionButtonPannel;
    bool showPlantSelection;
} UI;

void ui_init(UI *ui, Vector2 *screenSize);
Command ui_processInput(UI *ui, InputManager *input);
void ui_updateCursor(UI *ui, enum GameplayMode gameplayMode);
void ui_draw(UI *ui,
    const InputManager *input,
    const Vector2 *screenSize,
    const Garden *garden,
    enum GameplayMode gameplayMode);
