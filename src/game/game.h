#pragma once

#include "../ui/key_map.h"
#include "../ui/ui.h"
#include "gameplay.h"
#include <raylib.h>

// ProtoScenes
enum GameState { GAME_STATE_MAIN_MENU, GAME_STATE_GARDEN };

typedef struct Game {
    Garden garden;
    enum GameState state;
    UI ui;
    KeyMap keyMap;
    InputManager input;
    RenderTexture2D target;
    Vector2 screenSize;
    float scale;
    Vector2 screenOffset;
    enum GardeningTool toolSelected;
    int toolVariantsSelection[GARDENING_TOOL_COUNT];
    GameplaySpeed gameplaySpeed;
    float inGameSeconds;
} Game;

void game_init(Game *game);
void game_processInput(Game *game);
void game_update(Game *game, float deltaTime);
void game_draw(Game *game);
