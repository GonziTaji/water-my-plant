#include "button_pannel.h"
#include "../core/asset_manager.h"
#include "button.h"
#include "commands.h"
#include "input_manager.h"
#include "raylib.h"
#include <assert.h>

void buttonPannel_init(ButtonPannel *bp,
    int cols,
    int rows,
    Vector2i buttonDimensions,
    Vector2i buttonSpacing,
    Vector2i padding,
    Vector2i origin,
    UIButton buttons[PANNEL_MAX_BUTTONS]) {

    bp->cols = cols;
    bp->rows = rows;
    bp->buttonDimensions = buttonDimensions;
    bp->buttonSpacing = buttonSpacing;
    bp->padding = padding;
    bp->origin = origin;
    bp->buttonsCount = cols * rows;

    assert(bp->buttonsCount <= PANNEL_MAX_BUTTONS);

    for (int i = 0; i < bp->buttonsCount; i++) {
        int col = i % cols;
        int row = i / cols;
        float x = origin.x + padding.x + col * (buttonDimensions.x + buttonSpacing.x);
        float y = origin.y + padding.y + row * (buttonDimensions.y + buttonSpacing.y);

        buttons[i].bounds = (Rectangle){x, y, buttonDimensions.x, buttonDimensions.y};
        buttons[i].isMouseOver = false;

        bp->buttons[i] = buttons[i];
    }
}

Command buttonPannel_processInput(ButtonPannel *bn, InputManager *input) {
    for (int i = 0; i < bn->buttonsCount; i++) {
        bn->buttons[i].isMouseOver = button_isMouseOver(&bn->buttons[i], input->worldMousePos);
    }

    if (input->mouseButtonPressed == MOUSE_BUTTON_LEFT) {
        for (int i = 0; i < bn->buttonsCount; i++) {
            if (bn->buttons[i].isMouseOver) {
                return bn->buttons[i].command;
            }
        }
    }

    return (Command){COMMAND_NONE};
}

void buttonPannel_draw(ButtonPannel *bp, int fontSize) {
    int buttonGridWidth = (bp->cols * bp->buttonDimensions.x) +
                          ((bp->cols - 1) * bp->buttonSpacing.x) + (bp->padding.x * 2);

    int buttonGridHeight = (bp->rows * bp->buttonDimensions.y) +
                           ((bp->rows - 1) * bp->buttonSpacing.y) + (bp->padding.y * 2);

    DrawRectangle(bp->origin.x, bp->origin.y, buttonGridWidth, buttonGridHeight, BLACK);

    for (int i = 0; i < bp->buttonsCount; i++) {
        Color buttonColor = bp->buttons[i].isMouseOver ? LIGHTGRAY : GRAY;

        DrawRectangle(bp->buttons[i].bounds.x,
            bp->buttons[i].bounds.y,
            bp->buttons[i].bounds.width,
            bp->buttons[i].bounds.height,
            buttonColor);

        Vector2 textSize = MeasureTextEx(uiFont, bp->buttons[i].label, fontSize, 0);
        Vector2 textPos = {
            bp->buttons[i].bounds.x + (bp->buttons[i].bounds.width - textSize.x) / 2,
            bp->buttons[i].bounds.y + (bp->buttons[i].bounds.height - textSize.y) / 2,
        };

        DrawTextEx(uiFont, bp->buttons[i].label, textPos, fontSize, 0, WHITE);
    }
}
