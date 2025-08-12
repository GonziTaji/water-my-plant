#include "inputmap.h"
#include "commands.h"
#include <stdio.h>

void registerCommand(InputMap *inputMap, CommandFunction command, int key) {
    if (inputMap->registeredCommandsCount == INPUT_MAP_CAPACITY - 1) {
        printf("Trying to register a command in a full inputMap");
        return;
    }

    inputMap->registeredCommands[inputMap->registeredCommandsCount] =
        (CommandInputPair){command, key};

    inputMap->registeredCommandsCount++;
}

void inputmap_init(InputMap *inputMap) {
    registerCommand(inputMap, &command_addPlant, KEY_A);
    registerCommand(inputMap, &command_removePlant, KEY_R);
    registerCommand(inputMap, &command_focusNextPlanter, KEY_N);
    registerCommand(inputMap, &command_focusNextPlanter, KEY_RIGHT);
    registerCommand(inputMap, &command_focusPreviousPlanter, KEY_P);
    registerCommand(inputMap, &command_focusPreviousPlanter, KEY_LEFT);
    registerCommand(inputMap, &command_irrigate, KEY_W);
    registerCommand(inputMap, &command_feed, KEY_F);
}

void inputmap_processInput(InputMap *inputMap, Garden *garden) {
    for (int i = 0; i < inputMap->registeredCommandsCount; i++) {
        if (IsKeyPressed(inputMap->registeredCommands[i].key)) {
            CommandFunction command = *inputMap->registeredCommands[i].command;
            command(garden);
        }
    }
}
