#include "button.h"
#include "raylib.h"

bool button_isMouseOver(UIButton *b) {
    return CheckCollisionPointRec(GetMousePosition(), b->bounds);
}
