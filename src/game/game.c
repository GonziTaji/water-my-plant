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
    keyMap_init(&game->keyMap);
}

void game_update(Game *game, float deltaTime) {
    calculateScaleAndOffset(game);

    inputManager_update(&game->input, game->scale, game->screenOffset);
    keyMap_processInput(&game->keyMap, &game->garden);
    ui_processInput(&game->ui, &game->input, &game->garden);

    garden_processClick(&game->garden, &game->input);
    garden_update(&game->garden, deltaTime);
}

void game_draw(Game *game) {
    Planter *selectedPlanter = &game->garden.planters[game->garden.selectedPlanter];

    // Draw scene in texture
    BeginTextureMode(game->target);

    ClearBackground(WHITE);

    ui_draw(&game->ui, selectedPlanter);
    garden_draw(&game->garden);

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
