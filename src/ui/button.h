#pragma once
#include "input.h"
#include "inputmap.h"
#include <raylib.h>

typedef struct {
    const char *label;
    CommandFunction command;
    Rectangle bounds;
    bool isMouseOver;
    int mouseButtonClicked;
} UIButton;

bool button_isMouseOver(UIButton *b, const Input *input);
