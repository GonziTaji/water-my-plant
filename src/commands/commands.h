#pragma once

#include <stdbool.h>
typedef struct Game Game;

enum COMMAND_TYPE {
    COMMAND_NONE = 0,
    /// Fallback for ui elements that are not interactive, to stop event propagation
    COMMAND_UI_CLICKED,
    COMMAND_TILE_CLICKED,
    COMMAND_TOOL_SELECTED,
    COMMAND_TOOL_VARIANT_SELECTED,
    COMMAND_TOOL_VARIANT_SELECT_NEXT,
    COMMAND_GAMEPLAY_CHANGE_SPEED,
};

// used to have more members and will probably will have more members eventually
typedef union {
    int selection;
} CommandArgs;

typedef struct {
    enum COMMAND_TYPE type;
    CommandArgs args;
} Command;

bool command_dispatchCommand(Command cmd, Game *g);
