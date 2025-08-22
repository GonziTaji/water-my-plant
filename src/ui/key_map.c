#include "key_map.h"
#include "commands.h"
#include <stdio.h>

void registerCommand(KeyMap *keyMap, CommandFunction command, int key) {
    if (keyMap->registeredCommandsCount == INPUT_MAP_CAPACITY - 1) {
        printf("Trying to register a command in a full keyMap");
        return;
    }

    keyMap->registeredCommands[keyMap->registeredCommandsCount] = (CommandInputPair){command, key};

    keyMap->registeredCommandsCount++;
}

void keyMap_init(KeyMap *keyMap) {
    keyMap->registeredCommandsCount = 0;

    registerCommand(keyMap, &command_addPlanter, KEY_M);
    registerCommand(keyMap, &command_removePlanter, KEY_C);
    registerCommand(keyMap, &command_addPlant, KEY_A);
    registerCommand(keyMap, &command_removePlant, KEY_R);
    registerCommand(keyMap, &command_focusNextTile, KEY_N);
    registerCommand(keyMap, &command_focusNextTile, KEY_RIGHT);
    registerCommand(keyMap, &command_focusPreviousTile, KEY_P);
    registerCommand(keyMap, &command_focusPreviousTile, KEY_LEFT);
    registerCommand(keyMap, &command_irrigate, KEY_W);
    registerCommand(keyMap, &command_feed, KEY_F);
}

void keyMap_processInput(KeyMap *keyMap, Garden *garden) {
    for (int i = 0; i < keyMap->registeredCommandsCount; i++) {
        if (IsKeyPressed(keyMap->registeredCommands[i].key)) {
            CommandFunction command = *keyMap->registeredCommands[i].command;
            command(garden);
        }
    }
}
