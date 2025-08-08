#include "../game/game.h"
#include "raylib.h"

int main(void) {
    SetTargetFPS(144);

    Vector2 screenSize = {800, 600};
    InitWindow(screenSize.x, screenSize.y, "My little plant");

    int loops = 0;
    int maxLoops = 0;

    Game g;
    game_init(&g);

    /* 
     * MAIN LOOP
     */
    while ((maxLoops == 0 || loops < maxLoops) && !WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        game_update(&g, deltaTime);

        // Draw
        BeginDrawing();
        ClearBackground(WHITE);

        game_draw(&g);

        EndDrawing();

        loops++;
    }

    return 0;
}
