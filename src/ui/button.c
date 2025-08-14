#include "button.h"
#include "raylib.h"

bool button_isMouseOver(UIButton *b, const Input *input) {
    return CheckCollisionPointRec(input->mousePosVirtual, b->bounds);
}
