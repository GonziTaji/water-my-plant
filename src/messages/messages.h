#pragma once

#include <stdbool.h>
typedef struct Game Game;

// Events and Commands are treated the same
typedef enum {
    MESSAGE_NONE = 0,
    // events (change name?)
    /// Fallback for ui elements that are not interactive, to stop event propagation
    MESSAGE_EV_UI_CLICKED,
    MESSAGE_EV_TILE_CLICKED,

    // commands
    MESSAGE_CMD_TOOL_SELECT,
    MESSAGE_CMD_TOOL_VARIANT_SELECT,
    MESSAGE_CMD_TOOL_VARIANT_SELECT_NEXT,
    MESSAGE_CMD_GAMEPLAY_SPEED_CHANGE,
} MessageType;

// used to have more members and will probably will have more members eventually
typedef union {
    int selection;
} MessageArgs;

typedef struct {
    MessageType type;
    MessageArgs args;
} Message;

bool messages_dispatchMessage(Message msg, Game *g);
