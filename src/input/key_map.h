#pragma once

#include "../input/input.h"
#include "../messages/messages.h"

#define INPUT_MAP_CAPACITY 64

typedef struct {
    Message command;
    int key;
} Keybind;

typedef struct {
    Keybind registeredCommands[INPUT_MAP_CAPACITY];
    int registeredCommandsCount;
} KeyMap;

void keyMap_init(KeyMap *keyMap);
Message keyMap_processInput(KeyMap *keyMap, InputManager *input);
