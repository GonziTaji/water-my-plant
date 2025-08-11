#include "game.h"
#include "../entity/garden.h"
#include "../ui/ui.h"

void game_init(Game *game) {
    garden_init(&game->garden);
    ui_init(&game->ui);
}

void game_update(Game *game, float deltaTime) {
    ui_processInput(&game->ui, &game->garden);
    garden_update(&game->garden, deltaTime);
}

void game_draw(Game *game) {
    Planter *selectedPlanter = &game->garden.planters[game->garden.selectedPlanter];

    ui_draw(&game->ui, selectedPlanter);
    garden_draw(&game->garden);
}
