#include "button.h"
#include "raylib.h"

bool button_isMouseOver(UIButton *b, Vector2 mousePos) {
    return CheckCollisionPointRec(mousePos, b->bounds);
}

UIButton button_create(enum ButtonType type, ButtonContent content, Command cmd) {

    return (UIButton){
        .type = type,
        .content = content,
        .command = cmd,
        .isActive = false,
        .isMouseOver = false,
    };
}
