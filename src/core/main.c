#include "../game/game.h"
#include "raylib.h"

int main(void) {
    SetTargetFPS(144);

    Vector2 windowSize = {0, 0};

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowSize.x, windowSize.y, "My little plant");

    plant_loadTextures();

    Game g;
    game_init(&g);

    /*
     * MAIN LOOP
     */
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        game_update(&g, deltaTime);
        game_draw(&g);
    }

    plant_unloadTextures();
    game_unload(&g);

    return 0;
}
