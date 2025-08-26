#pragma once
#include "input_manager.h"
#include "key_map.h"
#include <raylib.h>

typedef struct {
    const char *label;
    CommandFunction command;
    Rectangle bounds;
    bool isMouseOver;
} UIButton;

bool button_isMouseOver(UIButton *b, const InputManager *input);
