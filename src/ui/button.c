#include "button.h"
#include "raylib.h"

bool button_isMouseOver(UIButton *b, const InputManager *input) {
    return CheckCollisionPointRec(input->worldMousePos, b->bounds);
}
