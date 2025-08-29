#pragma once
#include "../commands/commands.h"
#include <raylib.h>

enum ButtonType {
    BUTTON_TYPE_TEXT_LABEL,
    BUTTON_TYPE_SPRITE,
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
    bool isMouseOver;
    bool isActive;
} UIButton;

bool button_isMouseOver(UIButton *b, Vector2 mousePos);
UIButton button_create(enum ButtonType type, ButtonContent content, Command cmd);
