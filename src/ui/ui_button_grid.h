#pragma once
#include "button.h"
#include "input_manager.h"

// to avoid dynamic array and malloc. The memory "wasted" is negligible
#define UI_BUTTON_PANNEL_MAX_BUTTONS 20

typedef struct {
    int x, y;
} Vector2i;

typedef struct {
    // config
    int rows, cols;
    Vector2i buttonDimensions, buttonSpacing, padding, origin;
    // buttons
    UIButton buttons[UI_BUTTON_PANNEL_MAX_BUTTONS];
    int buttonsCount;
} UIButtonGrid;

void uiButtonGrid_init(UIButtonGrid *bp,
    int cols,
    int rows,
    Vector2i buttonDimensions,
    Vector2i buttonSpacing,
    Vector2i padding,
    Vector2i origin,
    UIButton buttons[UI_BUTTON_PANNEL_MAX_BUTTONS]);
Command uiButtonGrid_processInput(UIButtonGrid *bn, InputManager *input);
void uiButtonGrid_draw(UIButtonGrid *bp, int fontSize);
void uiButtonGrid_tranform(UIButtonGrid *bp, Vector2 newPos);
int uiButtonGrid_getWidth(UIButtonGrid *bp);
int uiButtonGrid_getHeight(UIButtonGrid *bp);
