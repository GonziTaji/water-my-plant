#include "game.h"
#include "../core/asset_manager.h"
#include "../entity/garden.h"
#include "../ui/ui.h"
#include "gameplay.h"
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

    game->gameplaySpeed = GAMEPLAY_SPEED_NORMAL;
    game->toolSelected = GARDENING_TOOL_NONE;
    game->screenSize = screenSize;
    game->target = LoadRenderTexture(screenSize.x, screenSize.y);
    game->state = GAME_STATE_MAIN_MENU;
    game->inGameSeconds = 0; // (23 * 60 * 60) + (60 * 59);

    for (int i = 0; i < GARDENING_TOOL_COUNT; i++) {
        game->toolVariantsSelection[i] = 0;
    }

    calculateScaleAndOffset(game);

    SetTextureFilter(game->target.texture, TEXTURE_FILTER_BILINEAR);

    garden_init(&game->garden, &game->screenSize, game->inGameSeconds);

    ui_init(&game->ui, &screenSize, game->gameplaySpeed);
    keyMap_init(&game->keyMap);
}

void game_processInput(Game *game) {
    input_update(&game->input, game->scale, game->screenOffset);

    Message cmd = keyMap_processInput(&game->keyMap, &game->input);
    if (messages_dispatchMessage(cmd, game)) {
        return;
    }

    cmd = ui_processInput(&game->ui, &game->input, game->toolSelected);
    if (messages_dispatchMessage(cmd, game)) {
        return;
    }

    cmd = garden_processInput(&game->garden, &game->input);
    if (messages_dispatchMessage(cmd, game)) {
        return;
    }
}

void game_update(Game *game, float deltaTime) {
    calculateScaleAndOffset(game);

    // 1.0, 1.5, 3.0
    const float speedFactor = (game->gameplaySpeed * game->gameplaySpeed + 2) * 0.5f;
    deltaTime *= speedFactor;

    switch (game->state) {
    case GAME_STATE_MAIN_MENU:
        if (IsKeyPressed(KEY_SPACE)) {
            game->state = GAME_STATE_GARDEN;
        }
        break;
    case GAME_STATE_GARDEN:
        game->inGameSeconds += GAME_SECONDS_PER_RL_SECONDS * deltaTime;

        if (game->inGameSeconds > SECONDS_IN_A_DAY) {
            game->inGameSeconds = 0;
        }

        garden_update(&game->garden, deltaTime, game->inGameSeconds);
        break;
    }
}

void drawGardenScene(Game *game) {
    ClearBackground((Color){100, 100, 100, 100});

    garden_draw(&game->garden, game->toolSelected, game->toolVariantsSelection[game->toolSelected]);

    // For debug
    input_drawMousePos(&game->input, game->screenSize);

    // UI must be at the end
    ui_draw(&game->ui,
        &game->input,
        &game->screenSize,
        &game->garden,
        game->toolSelected,
        game->inGameSeconds);
}

void drawMainMenuScene(Game *game) {
    ClearBackground(BLACK);

    int fontSize = uiFont.baseSize * 3.0f;
    const char *text = "Press space to start!";
    Vector2 textSize = MeasureTextEx(uiFont, text, fontSize, 0);
    Vector2 textPos = {
        (game->screenSize.x - textSize.x) / 2,
        (game->screenSize.y - textSize.y) / 2,
    };

    DrawTextEx(uiFont, text, textPos, fontSize, 0, WHITE);
}

void game_draw(Game *game) {
    // Draw in target render texture
    BeginTextureMode(game->target);

    switch (game->state) {

    case GAME_STATE_MAIN_MENU:
        drawMainMenuScene(game);
        break;

    case GAME_STATE_GARDEN:
        drawGardenScene(game);
        break;
    }

    EndTextureMode();

    // Draw scene in game texture
    BeginDrawing();

    ClearBackground(BLACK);

    Rectangle source
        = {0.0f, 0.0f, (float)game->target.texture.width, (float)-game->target.texture.height};

    Rectangle dest = {game->screenOffset.x,
        game->screenOffset.y,
        (float)game->screenSize.x * game->scale,
        (float)game->screenSize.y * game->scale};

    Vector2 origin = {0, 0};

    DrawTexturePro(game->target.texture, source, dest, origin, 0.0f, WHITE);

    EndDrawing();
}
