#pragma once
#include "commands.h"
#include "input_manager.h"
#include <raylib.h>

typedef struct {
    const char *label;
    Command command;
    Rectangle bounds;
    bool isMouseOver;
} UIButton;

bool button_isMouseOver(UIButton *b, const InputManager *input);
