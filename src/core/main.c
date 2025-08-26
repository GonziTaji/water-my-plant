#include "../game/game.h"
#include "asset_manager.h"
#include "raylib.h"
#include <stdio.h>

int main(void) {
    SetTargetFPS(144);

    Vector2 windowSize = {0, 0};

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowSize.x, windowSize.y, "My little plant");

    assetManager_loadAssets();

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

    printf(">>>>>Unloading\n");
    assetManager_unloadAssets();
    printf(">>>>>Unloaded\n");

    return 0;
}
