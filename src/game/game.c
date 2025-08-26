#include "game.h"
#include "../core/asset_manager.h"
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
    Vector2 screenSize = {1920, 1080};

    game->screenSize = screenSize;
    game->target = LoadRenderTexture(screenSize.x, screenSize.y);
    game->state = GAME_STATE_MAIN_MENU;

    calculateScaleAndOffset(game);

    SetTextureFilter(game->target.texture, TEXTURE_FILTER_BILINEAR);

    garden_init(&game->garden);
    ui_init(&game->ui, &screenSize);
    keyMap_init(&game->keyMap);
}

void game_update(Game *game, float deltaTime) {
    calculateScaleAndOffset(game);

    inputManager_update(&game->input, game->scale, game->screenOffset);

    switch (game->state) {
    case GAME_STATE_MAIN_MENU:
        if (IsKeyPressed(KEY_SPACE)) {
            game->state = GAME_STATE_GARDEN;
        }
        break;
    case GAME_STATE_GARDEN:
        keyMap_processInput(&game->keyMap, &game->garden);
        ui_processInput(&game->ui, &game->input, &game->garden);

        garden_processClick(&game->garden, &game->input);
        garden_update(&game->garden, deltaTime);
        break;
    }
}

void game_draw(Game *game) {
    // Draw scene in texture
    BeginTextureMode(game->target);

    switch (game->state) {
    case GAME_STATE_MAIN_MENU:
        ClearBackground(BLACK);
        // TODO: change UI with some mechanism an draw here
        int fontSize = uiFont.baseSize * 2.0f;
        const char *text = "Press space to start!";
        Vector2 textSize = MeasureTextEx(uiFont, text, fontSize, 0);
        Vector2 textPos = {
            (game->screenSize.x - textSize.x) / 2,
            (game->screenSize.y - textSize.y) / 2,
        };

        DrawTextEx(uiFont, text, textPos, fontSize, 0, WHITE);

        break;
    case GAME_STATE_GARDEN:
        ClearBackground((Color){185, 131, 131, 100});
        garden_draw(&game->garden);
        // UI must be at the end
        ui_draw(&game->ui, &game->screenSize, &game->garden);
        inputManager_drawMousePos(&game->input, game->screenSize);
        break;
    }

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
