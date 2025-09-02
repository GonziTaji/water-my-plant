#include "key_map.h"
#include "input_manager.h"
#include <raylib.h>
#include <stdio.h>

void registerCommand(KeyMap *keyMap, int key, Command cmd) {
    if (keyMap->registeredCommandsCount == INPUT_MAP_CAPACITY - 1) {
        printf("Trying to register a command in a full keyMap");
        return;
    }

    keyMap->registeredCommands[keyMap->registeredCommandsCount] = (Keybind){cmd, key};

    keyMap->registeredCommandsCount++;
}

void keyMap_init(KeyMap *keyMap) {
    keyMap->registeredCommandsCount = 0;

    registerCommand(
        keyMap, KEY_M, (Command){COMMAND_TOOL_SELECTED, {.tool = GARDENING_TOOL_PLANTER}});
    registerCommand(
        keyMap, KEY_A, (Command){COMMAND_TOOL_SELECTED, {.tool = GARDENING_TOOL_PLANT_CUTTING}});
    registerCommand(
        keyMap, KEY_R, (Command){COMMAND_TOOL_SELECTED, {.tool = GARDENING_TOOL_TRASH_BIN}});
    registerCommand(
        keyMap, KEY_W, (Command){COMMAND_TOOL_SELECTED, {.tool = GARDENING_TOOL_IRRIGATOR}});
    registerCommand(
        keyMap, KEY_F, (Command){COMMAND_TOOL_SELECTED, {.tool = GARDENING_TOOL_NUTRIENTS}});
    registerCommand(
        keyMap, KEY_ESCAPE, (Command){COMMAND_TOOL_SELECTED, {.tool = GARDENING_TOOL_NONE}});

    registerCommand(keyMap, KEY_TAB, (Command){COMMAND_PLANT_TYPE_SELECT_NEXT});
}

Command keyMap_processInput(KeyMap *keyMap, InputManager *input) {
    for (int i = 0; i < keyMap->registeredCommandsCount; i++) {
        if (input->keyPressed == keyMap->registeredCommands[i].key) {
            return keyMap->registeredCommands[i].command;
        }
    }

    return (Command){COMMAND_NONE};
}
