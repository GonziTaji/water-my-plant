#pragma once
#include "../entity/garden.h"
#include "../entity/planter.h"
#include "button.h"

#define UI_MAX_BUTTONS_COUNT 2

typedef struct {
    UIButton buttons[UI_MAX_BUTTONS_COUNT];
    int buttonsCount;
} UI;

void ui_init(UI *ui);
void ui_processInput(UI *ui, Garden *plant);
void ui_draw(UI *ui, Planter *selectedPlanter);
