#pragma once
#include "../entity/garden.h"
#include "../entity/planter.h"
#include "button_pannel.h"

typedef struct {
    ButtonPannel mainButtonPannel;
    ButtonPannel plantSelectionButtonPannel;
} UI;

void ui_init(UI *ui, Vector2 *screenSize);
void ui_processInput(UI *ui, InputManager *input, Garden *garden);
void ui_draw(UI *ui, const Vector2 *screenSize, const Garden *garden);
