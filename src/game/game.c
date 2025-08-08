#include "game.h"
#include "../entity/plant.h"
#include "../ui/ui.h"

void game_init(Game *game) {
    plant_init(&game->plant);
    ui_init(&game->ui);
}

void game_update(Game *game, float deltaTime) {
    ui_update(&game->ui, &game->plant);
    plant_update(&game->plant, deltaTime);
}

void game_draw(Game *game) {
    ui_draw(&game->ui);
    plant_draw(&game->plant);
}
