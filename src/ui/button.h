#pragma once
#include "inputmap.h"
#include <raylib.h>

typedef struct {
    Rectangle bounds;
    const char *label;
    bool isMouseOver;
    int mouseButtonClicked;
    CommandFunction command;
} UIButton;

bool button_isMouseOver(UIButton *b);
