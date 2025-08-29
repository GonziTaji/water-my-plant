#include "button_pannel.h"
#include "../core/asset_manager.h"
#include "button.h"
#include "input_manager.h"
#include "raylib.h"
#include <assert.h>

Rectangle calculateButtonPos(ButtonPannel *bp, int buttonIndex) {
    int col = buttonIndex % bp->cols;
    int row = buttonIndex / bp->cols;
    float x = bp->origin.x + bp->padding.x + col * (bp->buttonDimensions.x + bp->buttonSpacing.x);
    float y = bp->origin.y + bp->padding.y + row * (bp->buttonDimensions.y + bp->buttonSpacing.y);

    return (Rectangle){x, y, bp->buttonDimensions.x, bp->buttonDimensions.y};
}

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
        buttons[i].bounds = calculateButtonPos(bp, i);
        buttons[i].isMouseOver = false;

        bp->buttons[i] = buttons[i];
    }
}

void buttonPannel_translate(ButtonPannel *bp, Vector2 newPos) {
    bp->origin = (Vector2i){newPos.x, newPos.y};

    for (int i = 0; i < bp->buttonsCount; i++) {
        bp->buttons[i].bounds = calculateButtonPos(bp, i);
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

        if (bp->buttons[i].type == BUTTON_TYPE_SPRITE) {
            Rectangle dest = {
                bp->buttons[i].bounds.x,
                bp->buttons[i].bounds.y,
                bp->buttons[i].bounds.width,
                bp->buttons[i].bounds.height,
            };

            // Vector2 pivot = {dest.width / 2, dest.height};
            Vector2 pivot = {0, 0};

            DrawTexturePro(bp->buttons[i].content.icon.sprite,
                bp->buttons[i].content.icon.sourceRect,
                dest,
                pivot,
                0,
                WHITE);

        } else {
            Vector2 textSize = MeasureTextEx(uiFont, bp->buttons[i].content.label, fontSize, 0);
            Vector2 textPos = {
                bp->buttons[i].bounds.x + (bp->buttons[i].bounds.width - textSize.x) / 2,
                bp->buttons[i].bounds.y + (bp->buttons[i].bounds.height - textSize.y) / 2,
            };

            DrawTextEx(uiFont, bp->buttons[i].content.label, textPos, fontSize, 0, WHITE);
        }
    }
}
