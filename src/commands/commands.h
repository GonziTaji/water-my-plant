#pragma once

#include "../entity/plant.h"
#include "../game/gameplay.h"

typedef struct Game Game;

enum COMMAND_TYPE {
    COMMAND_NONE = 0,
    COMMAND_TILE_CLICKED,
    COMMAND_TOOL_SELECTED,
    COMMAND_ADD_PLANT,
};

typedef union {
    enum PlantType plantType;
    int tileIndex;
    enum GardeningTool tool;
} CommandArgs;

typedef struct {
    enum COMMAND_TYPE type;
    CommandArgs args;
} Command;

bool command_dispatchCommand(Command cmd, Game *g);
