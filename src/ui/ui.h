#pragma once
#include "../entity/garden.h"
#include "../entity/planter.h"
#include "button_pannel.h"

typedef struct Game Game;

typedef struct {
    ButtonPannel mainButtonPannel;
    ButtonPannel plantSelectionButtonPannel;
    bool showPlantSelection;
} UI;

void ui_init(UI *ui, Vector2 *screenSize);
Command ui_processInput(UI *ui, InputManager *input);
void ui_draw(UI *ui, const Vector2 *screenSize, const Garden *garden);
