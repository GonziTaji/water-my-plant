#include "key_map.h"
#include "../game/gameplay.h"
#include "input.h"
#include <raylib.h>
#include <stdio.h>

void registerCommand(KeyMap *keyMap, int key, Message cmd) {
    if (keyMap->registeredCommandsCount == INPUT_MAP_CAPACITY - 1) {
        printf("Trying to register a command in a full keyMap");
        return;
    }

    keyMap->registeredCommands[keyMap->registeredCommandsCount] = (Keybind){cmd, key};

    keyMap->registeredCommandsCount++;
}

void keyMap_init(KeyMap *keyMap) {
    keyMap->registeredCommandsCount = 0;

    Message toolSelectionCommands[GARDENING_TOOL_COUNT];
    for (int i = 0; i < GARDENING_TOOL_COUNT; i++) {
        toolSelectionCommands[i].type = MESSAGE_CMD_TOOL_SELECT;
        toolSelectionCommands[i].args.selection = i;
    }

    // this will need to change if the player can change the order of the tools
    for (int i = 0; i < GARDENING_TOOL_COUNT; i++) {
        registerCommand(keyMap, KEY_ONE + i, toolSelectionCommands[i]);

        // the hotkey bar can only have 9 tools (for now)
        if (KEY_ONE + i == KEY_NINE) {
            break;
        }
    }

    registerCommand(keyMap, KEY_M, toolSelectionCommands[GARDENING_TOOL_PLANTER]);
    registerCommand(keyMap, KEY_A, toolSelectionCommands[GARDENING_TOOL_PLANT_CUTTING]);
    registerCommand(keyMap, KEY_R, toolSelectionCommands[GARDENING_TOOL_TRASH_BIN]);
    registerCommand(keyMap, KEY_W, toolSelectionCommands[GARDENING_TOOL_IRRIGATOR]);
    registerCommand(keyMap, KEY_F, toolSelectionCommands[GARDENING_TOOL_NUTRIENTS]);

    registerCommand(keyMap, KEY_ESCAPE, toolSelectionCommands[GARDENING_TOOL_NONE]);

    registerCommand(keyMap, KEY_TAB, (Message){MESSAGE_CMD_TOOL_VARIANT_SELECT_NEXT});
}

Message keyMap_processInput(KeyMap *keyMap, InputManager *input) {
    for (int i = 0; i < keyMap->registeredCommandsCount; i++) {
        if (input->keyPressed == keyMap->registeredCommands[i].key) {
            return keyMap->registeredCommands[i].command;
        }
    }

    // Mouse global binds. Make a map if this grows or it's configurable
    if (input->mouseButtonPressed == MOUSE_RIGHT_BUTTON) {
        return (Message){MESSAGE_CMD_TOOL_SELECT, {.selection = GARDENING_TOOL_NONE}};
    }

    return (Message){MESSAGE_NONE};
}
