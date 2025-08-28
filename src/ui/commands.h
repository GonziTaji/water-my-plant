#pragma once

#include "../entity/plant.h"

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
    COMMAND_IRRIGATION_MODE,
    COMMAND_TILE_CLICKED,
};

typedef union {
    enum PLANT_TYPE plantType;
    int tileIndex;
} CommandArgs;

typedef struct {
    enum COMMAND_TYPE type;
    CommandArgs args;
} Command;

bool command_dispatchCommand(Command cmd, Game *g);
