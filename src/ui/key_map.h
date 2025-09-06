#pragma once

#include "../commands/commands.h"
#include "input_manager.h"

#define INPUT_MAP_CAPACITY 16

typedef struct {
    Command command;
    int key;
} Keybind;

typedef struct {
    Keybind registeredCommands[INPUT_MAP_CAPACITY];
    int registeredCommandsCount;
} KeyMap;

void keyMap_init(KeyMap *keyMap);
Command keyMap_processInput(KeyMap *keyMap, InputManager *input);
