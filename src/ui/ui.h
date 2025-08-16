#pragma once
#include "../entity/garden.h"
#include "../entity/planter.h"
#include "button.h"

#define BUTTON_ROWS 6
#define BUTTON_COLS 1
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 80
#define BUTTON_SPACING_X 8
#define BUTTON_SPACING_Y 8
#define BUTTON_GRID_PADDING_X 8
#define BUTTON_GRID_PADDING_Y 8
#define BUTTON_GRID_ORIGIN_X 20
#define BUTTON_GRID_ORIGIN_Y 100

typedef struct {
    UIButton buttons[BUTTON_ROWS * BUTTON_COLS];
    int buttonsCount;
    Font font;
} UI;

void ui_init(UI *ui);
void ui_processInput(UI *ui, const InputManager *input, Garden *plant);
void ui_draw(UI *ui, const Vector2 *screenSize, const Planter *selectedPlanter);
void ui_unloadResources(UI *ui);
