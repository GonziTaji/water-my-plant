#include "button.h"
#include "raylib.h"

bool button_isMouseOver(UIButton *b, Vector2 mousePos) {
    return CheckCollisionPointRec(mousePos, b->bounds);
}
