#pragma once
#include <raylib.h>

typedef enum {
    BUTTON_ACTION_IRRIGATE,
    BUTTON_ACTION_FEED,
} ButtonActionType;

typedef struct {
    Rectangle bounds;
    const char *label;
    bool isMouseOver;
    int mouseButtonClicked;
    ButtonActionType buttonAction;
} UIButton;

bool button_isMouseOver(UIButton *b);
