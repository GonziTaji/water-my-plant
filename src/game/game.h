#pragma once
#include "../entity/plant.h"
#include "../ui/ui.h"

typedef struct {
    Plant plant;
    UI ui;
} Game;

void game_init(Game *game);
void game_update(Game *game, float deltaTime);
void game_draw(Game *game);
