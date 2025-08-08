#pragma once
#include "button.h"
#include "../entity/plant.h"

enum { UI_MAX_BUTTON_COUNT = 2 };

typedef struct {
    UIButton buttons[UI_MAX_BUTTON_COUNT];
    int buttonsCount;
} UI;

void ui_init(UI *ui);
void ui_update(UI *ui, Plant *plant);
void ui_draw(UI *ui);
