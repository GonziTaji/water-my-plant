#pragma once
#include "../commands/commands.h"
#include <raylib.h>

enum ButtonType {
    BUTTON_TYPE_TEXT_LABEL,
    BUTTON_TYPE_SPRITE,
};

enum ButtonStatus {
    BUTTON_STATUS_NORMAL,
    BUTTON_STATUS_CLICK,
    BUTTON_STATUS_HOVER,
    BUTTON_STATUS_ACTIVE
};

typedef union {
    struct {
        Texture2D sprite;
        Rectangle sourceRect;
    } icon;
    const char *label;
} ButtonContent;

typedef struct {
    enum ButtonType type;
    ButtonContent content;
    Command command;
    Rectangle bounds;
    enum ButtonStatus status;
} UIButton;

bool button_isMouseOver(UIButton *b, Vector2 mousePos);
