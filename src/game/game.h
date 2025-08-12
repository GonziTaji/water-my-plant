#pragma once
#include "../entity/plant.h"
#include "../ui/inputmap.h"
#include "../ui/ui.h"

typedef struct {
    Garden garden;
    UI ui;
    InputMap inputMap;
} Game;

void game_init(Game *game);
void game_update(Game *game, float deltaTime);
void game_draw(Game *game);
