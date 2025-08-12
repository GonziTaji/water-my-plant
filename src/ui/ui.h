#pragma once
#include "../entity/garden.h"
#include "../entity/planter.h"
#include "button.h"

#define BUTTON_ROWS 1
#define BUTTON_COLS 6
#define BUTTON_WIDTH 120
#define BUTTON_HEIGHT 40
#define BUTTON_SPACING_X 2
#define BUTTON_SPACING_Y 2
#define BUTTON_GRID_ORIGIN_X 2
#define BUTTON_GRID_ORIGIN_Y 0

typedef struct {
    UIButton buttons[BUTTON_ROWS * BUTTON_COLS];
    int buttonsCount;
} UI;

void ui_init(UI *ui);
void ui_processInput(UI *ui, Garden *plant);
void ui_draw(UI *ui, Planter *selectedPlanter);
