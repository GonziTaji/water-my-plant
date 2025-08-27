#pragma once

#include "../entity/garden.h"

typedef struct Game Game;

enum COMMAND_TYPE {
    COMMAND_NONE = 0,
    COMMAND_FOCUS_NEXT_TILE,
    COMMAND_FOCUS_PREV_TILE,
    COMMAND_ADD_PLANTER,
    COMMAND_REMOVE_PLANTER,
    COMMAND_ADD_PLANT,
    COMMAND_OPEN_PLANT_SELECTION,
    COMMAND_REMOVE_PLANT,
    COMMAND_IRRIGATE,
    COMMAND_FEED,
};

typedef union {
    enum PLANT_TYPE plantType;
} CommandArgs;

typedef struct {
    enum COMMAND_TYPE type;
    CommandArgs args;
} Command;

void command_dispatchCommand(Command cmd, Game *g);
