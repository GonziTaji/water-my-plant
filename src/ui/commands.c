#include "commands.h"
#include "../core/asset_manager.h"
#include "../entity/garden.h"
#include "../game/game.h"
#include "input_manager.h"

// Utils

// TODO: move functions to garden?
bool gardenHasPlanterSelected(Garden *garden) {
    return garden->tileSelected != -1 && garden->tiles[garden->tileSelected].hasPlanter;
}

Planter *getSelectedPlanter(Garden *garden) {
    return &garden->tiles[garden->tileSelected].planter;
}

// Commands

void command_addPlanter(Garden *garden) {
    if (garden->tileSelected == -1 && garden->tiles[garden->tileSelected].hasPlanter) {
        return;
    }

    garden->tiles[garden->tileSelected].hasPlanter = true;
    planter_init(getSelectedPlanter(garden));
}

void command_removeFromTile(Garden *garden) {
    if (!gardenHasPlanterSelected(garden)) {
        return;
    }

    Planter *planter = getSelectedPlanter(garden);

    if (!planter->hasPlant) {
        garden->tiles[garden->tileSelected].hasPlanter = false;
        return;
    } else {
        planter_removePlant(planter);
    }
}

void command_focusNextTile(Garden *garden) {
    if (garden->tileSelected == -1 || garden->tileSelected == garden->tilesCount - 1) {
        garden->tileSelected = 0;
    } else {
        garden->tileSelected = garden->tileSelected + 1;
    }
}

void command_focusPreviousTile(Garden *garden) {
    if (garden->tileSelected == -1 || garden->tileSelected == 0) {
        garden->tileSelected = garden->tilesCount - 1;
    } else {
        garden->tileSelected = garden->tileSelected - 1;
    }
}

void command_requestPlantTypeToAdd(Garden *garden, UI *ui, InputManager *input, int tileIndex) {
    if (gardenHasPlanterSelected(garden)) {
        ui->showPlantSelection = true;

        buttonPannel_translate(&ui->plantSelectionButtonPannel, input->worldMousePos);
    }
}

void command_addPlant(Garden *garden, UI *ui, enum PLANT_TYPE type) {
    if (!gardenHasPlanterSelected(garden)) {
        return;
    }

    planter_addPlant(&garden->tiles[garden->tileSelected].planter, type);
    ui->showPlantSelection = false;
}

void command_irrigate(Garden *garden) {
    Planter *planter = getSelectedPlanter(garden);

    if (!gardenHasPlanterSelected(garden) || !planter->hasPlant) {
        return;
    }

    plant_irrigate(&planter->plant);
}

void command_feed(Garden *garden) {
    Planter *planter = getSelectedPlanter(garden);

    if (!gardenHasPlanterSelected(garden) || !planter->hasPlant) {
        return;
    }

    plant_feed(&planter->plant);
}

void command_changeTool(Game *g, enum GardeningTool tool) {
    if (g->toolSelected == tool) {
        g->toolSelected = GARDENING_TOOL_NONE;
    } else {
        g->toolSelected = tool;
    }
}

void command_tileClicked(Game *game, int tileIndex, enum GardeningTool toolSelected) {
    game->garden.tileSelected = tileIndex;

    switch (toolSelected) {

    case GARDENING_TOOL_IRRIGATOR:
        command_irrigate(&game->garden);
        break;

    case GARDENING_TOOL_NUTRIENTS:
        command_feed(&game->garden);
        break;

    case GARDENING_TOOL_PLANTER:
        command_addPlanter(&game->garden);
        break;

    case GARDENING_TOOL_PLANT_CUTTING:
        command_requestPlantTypeToAdd(&game->garden, &game->ui, &game->input, tileIndex);
        break;

    case GARDENING_TOOL_TRASH_BIN:
        command_removeFromTile(&game->garden);
        break;

    case GARDENING_TOOL_NONE:
        game->toolSelected = GARDENING_TOOL_NONE;
        break;
    }
}

/// returns `true` if a command was executed (cmd.type is not "COMMAND_NONE"
bool command_dispatchCommand(Command cmd, Game *g) {
    switch (cmd.type) {
    case COMMAND_FOCUS_NEXT_TILE:
        command_focusNextTile(&g->garden);
        break;

    case COMMAND_FOCUS_PREV_TILE:
        command_focusPreviousTile(&g->garden);
        break;

    case COMMAND_TILE_CLICKED:
        command_tileClicked(g, cmd.args.tileIndex, g->toolSelected);
        break;

    case COMMAND_TOOL_SELECTED:
        command_changeTool(g, cmd.args.tool);
        break;

    case COMMAND_ADD_PLANT:
        command_addPlant(&g->garden, &g->ui, cmd.args.plantType);
        break;

    case COMMAND_NONE:
        return false;
        break;
    }

    return true;
}
