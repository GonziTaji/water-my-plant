#include "../game/game.h"
#include "asset_manager.h"
#include "raylib.h"

int main(void) {
    SetTargetFPS(144);

    Vector2 windowSize = {0, 0};

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowSize.x, windowSize.y, "My little plant");
    SetExitKey(KEY_NULL);

    assetManager_loadAssets();

    Game g;
    game_init(&g);

    /*
     * MAIN LOOP
     */
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        game_processInput(&g);
        game_update(&g, deltaTime);
        game_draw(&g);
    }

    // Should we?
    assetManager_unloadAssets();

    return 0;
}
