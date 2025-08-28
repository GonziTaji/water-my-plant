#pragma once
#include "button.h"
#include "input_manager.h"

// to avoid dynamic array and malloc. The memory "wasted" is negligible
#define PANNEL_MAX_BUTTONS 20

typedef struct {
    int x, y;
} Vector2i;

typedef struct {
    // config
    int rows, cols;
    Vector2i buttonDimensions, buttonSpacing, padding, origin;
    // buttons
    UIButton buttons[PANNEL_MAX_BUTTONS];
    int buttonsCount;
} ButtonPannel;

void buttonPannel_init(ButtonPannel *bp,
    int cols,
    int rows,
    Vector2i buttonDimensions,
    Vector2i buttonSpacing,
    Vector2i padding,
    Vector2i origin,
    UIButton buttons[PANNEL_MAX_BUTTONS]);
Command buttonPannel_processInput(ButtonPannel *bn, InputManager *input);
void buttonPannel_draw(ButtonPannel *bp, int fontSize);
