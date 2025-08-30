#pragma once

#include "../entity/plant.h"
#include "../game/gameplay.h"

typedef struct Game Game;

enum COMMAND_TYPE {
    COMMAND_NONE = 0,
    /// Fallback for ui elements that are not interactive, to stop event propagation
    COMMAND_UI_CLICKED,
    COMMAND_TILE_CLICKED,
    COMMAND_TOOL_SELECTED,
    COMMAND_PLANT_TYPE_SELECTED,
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
