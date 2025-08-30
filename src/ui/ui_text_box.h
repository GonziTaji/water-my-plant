#pragma once
#include "../commands/commands.h"
#include "input_manager.h"

typedef struct {
    // config
    int fontSize;
    Rectangle bounds;
    Vector2 padding;
    Vector2 cursorPosition;
    bool isMouseOver;
} UITextBox;

void uiTextBox_init(UITextBox *bp, int fontSize, Rectangle bounds, Vector2 padding);
Command uiTextBox_processInput(UITextBox *bn, InputManager *input);
void uiTextBox_draw(UITextBox *bp, int fontSize);
void uiTextBox_translate(UITextBox *bp, Vector2 newPos);
void uiTextBox_drawTextLine(UITextBox *tb, const char *text, Color color);
