#pragma once
#include "../entity/garden.h"
#include "../entity/planter.h"
#include "button.h"

#define BUTTON_ROWS 8
#define BUTTON_COLS 1
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 80
#define BUTTON_SPACING_X 8
#define BUTTON_SPACING_Y 8
#define BUTTON_GRID_PADDING_X 8
#define BUTTON_GRID_PADDING_Y 8
#define BUTTON_GRID_ORIGIN_X 20
#define BUTTON_GRID_ORIGIN_Y 20

typedef struct {
    UIButton buttons[BUTTON_ROWS * BUTTON_COLS];
    int buttonsCount;
} UI;

void ui_init(UI *ui);
void ui_processInput(UI *ui, InputManager *input, Garden *garden);
void ui_draw(UI *ui, const Vector2 *screenSize, const Garden *garden);
