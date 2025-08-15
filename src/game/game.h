#pragma once
#include "../entity/plant.h"
#include "../ui/key_map.h"
#include "../ui/ui.h"
#include <raylib.h>

typedef struct {
    Garden garden;
    UI ui;
    KeyMap keyMap;
    InputManager input;
    RenderTexture2D target;
    Vector2 screenSize;
    float scale;
    Vector2 screenOffset;
} Game;

void game_init(Game *game);
void game_update(Game *game, float deltaTime);
void game_draw(Game *game);
void game_unload(Game *game);
