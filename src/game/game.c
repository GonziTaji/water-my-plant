#include "game.h"
#include "../entity/garden.h"
#include "../ui/ui.h"
#include <raylib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void calculateScaleAndOffset(Game *game) {
    game->scale = MIN((float)GetScreenWidth() / game->screenSize.x,
        (float)GetScreenHeight() / game->screenSize.y);

    game->screenOffset = (Vector2){
        (GetScreenWidth() - ((float)game->screenSize.x * game->scale)) * 0.5f,
        (GetScreenHeight() - ((float)game->screenSize.y * game->scale)) * 0.5f,
    };
}

void game_init(Game *game) {
    Vector2 screenSize = {800, 600};

    game->screenSize = screenSize;
    game->target = LoadRenderTexture(screenSize.x, screenSize.y);

    calculateScaleAndOffset(game);

    // SetTextureFilter(game->targetTexture.texture, TEXTURE_FILTER_BILINEAR);

    garden_init(&game->garden);
    ui_init(&game->ui);
    inputmap_init(&game->inputMap);
}

void game_update(Game *game, float deltaTime) {
    calculateScaleAndOffset(game);

    input_update(&game->input, game->scale, game->screenOffset);
    ui_processInput(&game->ui, &game->input, &game->garden);
    inputmap_processInput(&game->inputMap, &game->garden);
    garden_update(&game->garden, deltaTime);
}

void game_draw(Game *game) {
    Planter *selectedPlanter = &game->garden.planters[game->garden.selectedPlanter];

    // Draw in texture
    BeginTextureMode(game->target);

    ClearBackground(WHITE);

    ui_draw(&game->ui, selectedPlanter);
    garden_draw(&game->garden);

    // Vector2 mouse = GetMousePosition();
    // Vector2 virtualMouse = game->input.mousePosVirtual;

    // DrawText(
    //     TextFormat("Default Mouse: [%i , %i]", (int)mouse.x, (int)mouse.y), 350, 25, 20, GREEN);
    // DrawText(TextFormat("Virtual Mouse: [%i , %i]", (int)virtualMouse.x, (int)virtualMouse.y),
    //     350,
    //     55,
    //     20,
    //     YELLOW);

    EndTextureMode();

    // Draw in target render texture
    BeginDrawing();

    ClearBackground(BLACK);

    Rectangle source = {
        0.0f, 0.0f, (float)game->target.texture.width, (float)-game->target.texture.height};

    Rectangle dest = {game->screenOffset.x,
        game->screenOffset.y,
        (float)game->screenSize.x * game->scale,
        (float)game->screenSize.y * game->scale};

    Vector2 origin = {0, 0};

    DrawTexturePro(game->target.texture, source, dest, origin, 0.0f, WHITE);
    EndDrawing();
}

void game_unload(Game *game) {
    UnloadRenderTexture(game->target);
}
