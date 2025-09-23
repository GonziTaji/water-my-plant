#include "key_map.h"
#include "../game/gameplay.h"
#include "../utils/utils.h"
#include "input.h"
#include <assert.h>
#include <raylib.h>

void registerCommand(KeyMap *keyMap, int key, Message cmd) {
    assert(keyMap->registeredCommandsCount != INPUT_MAP_CAPACITY - 1
           && "Trying to register a command in a full keyMap");

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

    registerCommand(keyMap, KEY_EQUAL, (Message){MESSAGE_CMD_VIEW_ZOOM_UP});
    registerCommand(keyMap, KEY_MINUS, (Message){MESSAGE_CMD_VIEW_ZOOM_DOWN});
    registerCommand(keyMap, KEY_ZERO, (Message){MESSAGE_CMD_VIEW_ZOOM_RESET});
    registerCommand(keyMap, KEY_GRAVE, (Message){MESSAGE_CMD_VIEW_ROTATE});
    registerCommand(keyMap, KEY_T, (Message){MESSAGE_CMD_TOOL_VARIANT_ROTATE});
}

Message keyMap_processInput(KeyMap *keyMap, InputManager *input) {
    for (int i = 0; i < keyMap->registeredCommandsCount; i++) {
        if (input->keyPressed == keyMap->registeredCommands[i].key) {
            return keyMap->registeredCommands[i].command;
        }
    }

    if (input->mouseButtonState[MOUSE_BUTTON_RIGHT] == MOUSE_BUTTON_STATE_DRAGGING) {
        if (utils_absf(input->worldMouseDelta.x + input->worldMouseDelta.y) > 2) {
            return (Message){MESSAGE_CMD_VIEW_MOVE, {.vector = input->worldMouseDelta}};
        }
    }

    if (input->mouseButtonState[MOUSE_BUTTON_RIGHT] == MOUSE_BUTTON_STATE_RELEASED) {
        return (Message){MESSAGE_CMD_TOOL_SELECT, {.selection = GARDENING_TOOL_NONE}};
    }

    if (input->mouseWheelMove > 0) {
        return (Message){MESSAGE_CMD_VIEW_ZOOM_UP};

    } else if (input->mouseWheelMove < 0) {
        return (Message){MESSAGE_CMD_VIEW_ZOOM_DOWN};
    }

    return (Message){MESSAGE_NONE};
}
