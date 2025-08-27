#include "key_map.h"
#include "commands.h"
#include <stdio.h>

void registerCommand(KeyMap *keyMap, Command cmd, int key) {
    if (keyMap->registeredCommandsCount == INPUT_MAP_CAPACITY - 1) {
        printf("Trying to register a command in a full keyMap");
        return;
    }

    keyMap->registeredCommands[keyMap->registeredCommandsCount] = (Keybind){cmd, key};

    keyMap->registeredCommandsCount++;
}

void keyMap_init(KeyMap *keyMap) {
    keyMap->registeredCommandsCount = 0;

    registerCommand(keyMap, (Command){COMMAND_ADD_PLANTER}, KEY_M);
    registerCommand(keyMap, (Command){COMMAND_REMOVE_PLANTER}, KEY_C);
    registerCommand(keyMap, (Command){COMMAND_OPEN_PLANT_SELECTION}, KEY_A);
    registerCommand(keyMap, (Command){COMMAND_REMOVE_PLANT}, KEY_R);
    registerCommand(keyMap, (Command){COMMAND_FOCUS_NEXT_TILE}, KEY_N);
    registerCommand(keyMap, (Command){COMMAND_FOCUS_NEXT_TILE}, KEY_RIGHT);
    registerCommand(keyMap, (Command){COMMAND_FOCUS_PREV_TILE}, KEY_P);
    registerCommand(keyMap, (Command){COMMAND_FOCUS_PREV_TILE}, KEY_LEFT);
    registerCommand(keyMap, (Command){COMMAND_IRRIGATE}, KEY_W);
    registerCommand(keyMap, (Command){COMMAND_FEED}, KEY_F);
}

Command keyMap_processInput(KeyMap *keyMap) {
    for (int i = 0; i < keyMap->registeredCommandsCount; i++) {
        if (IsKeyPressed(keyMap->registeredCommands[i].key)) {
            return keyMap->registeredCommands[i].command;
        }
    }

    return (Command){COMMAND_NONE};
}
