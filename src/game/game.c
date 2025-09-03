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
    game->plantTypeSelected = PLANT_TYPE_CRASSULA_OVATA;
    game->planterTypeSelected = PLANTER_TYPE_2_X_2;
    game->screenSize = screenSize;
    game->target = LoadRenderTexture(screenSize.x, screenSize.y);
    game->state = GAME_STATE_MAIN_MENU;
    game->inGameSeconds = 0; // (23 * 60 * 60) + (60 * 59);

    calculateScaleAndOffset(game);

    SetTextureFilter(game->target.texture, TEXTURE_FILTER_BILINEAR);

    garden_init(&game->garden, screenSize, game->inGameSeconds);

    ui_init(&game->ui, &screenSize, game->gameplaySpeed);
    keyMap_init(&game->keyMap);
}

void game_processInput(Game *game) {
    inputManager_update(&game->input, game->scale, game->screenOffset);

    // One command per frame?
    if (game->input.mouseButtonPressed == MOUSE_RIGHT_BUTTON) {
        command_dispatchCommand(
            (Command){COMMAND_TOOL_SELECTED, {.tool = GARDENING_TOOL_NONE}}, game);
        return;
    }

    Command cmd = keyMap_processInput(&game->keyMap, &game->input);
    if (command_dispatchCommand(cmd, game)) {
        return;
    }

    cmd = ui_processInput(&game->ui, &game->input, game->toolSelected);
    if (command_dispatchCommand(cmd, game)) {
        return;
    }

    cmd = garden_processInput(&game->garden, &game->input);
    if (command_dispatchCommand(cmd, game)) {
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

void drawGarden(Game *game) {
    ClearBackground((Color){185, 131, 131, 100});

    garden_draw(&game->garden);
    // UI must be at the end
    ui_draw(&game->ui,
        &game->input,
        &game->screenSize,
        &game->garden,
        game->toolSelected,
        game->inGameSeconds);

    // For debug
    inputManager_drawMousePos(&game->input, game->screenSize);
}

void drawMainMenu(Game *game) {
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
        drawMainMenu(game);
        break;

    case GAME_STATE_GARDEN:
        drawGarden(game);
        break;
    }

    EndTextureMode();

    // Draw scene in game texture
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
