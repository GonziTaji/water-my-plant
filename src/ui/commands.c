#include "commands.h"
#include "../core/asset_manager.h"
#include "../entity/garden.h"
#include "../game/game.h"

// Utils

// TODO: move functions to garden?
bool gardenHasPlanterSelected(Garden *garden) {
    return garden->tileSelected != -1 && garden->tiles[garden->tileSelected].hasPlanter;
}

Planter *getSelectedPlanter(Garden *garden) {
    return &garden->tiles[garden->tileSelected].planter;
}

// Commands

void command_openPlantSelection(UI *ui, Garden *garden) {
    if (gardenHasPlanterSelected(garden) && !getSelectedPlanter(garden)->hasPlant) {
        ui->showPlantSelection = true;
    }
}

void command_addPlanter(Garden *garden) {
    if (garden->tileSelected == -1 && garden->tiles[garden->tileSelected].hasPlanter) {
        return;
    }

    garden->tiles[garden->tileSelected].hasPlanter = true;
    planter_init(getSelectedPlanter(garden));
}

void command_removePlanter(Garden *garden) {
    if (!gardenHasPlanterSelected(garden)) {
        return;
    }

    garden->tiles[garden->tileSelected].hasPlanter = false;
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

void command_addPlant(Garden *garden, UI *ui, enum PLANT_TYPE type) {
    if (!gardenHasPlanterSelected(garden)) {
        return;
    }

    planter_addPlant(&garden->tiles[garden->tileSelected].planter, type);
    ui->showPlantSelection = false;
}

void command_removePlant(Garden *garden) {
    Planter *planter = getSelectedPlanter(garden);

    if (!gardenHasPlanterSelected(garden) || !planter->hasPlant) {
        return;
    }

    if (garden->tileSelected == -1 || !garden->tiles[garden->tileSelected].hasPlanter ||
        !garden->tiles[garden->tileSelected].planter.hasPlant) {
        return;
    }

    planter_removePlant(planter);
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

void command_startIrrigationMode(Game *g) {
    if (g->gameplayMode == GAMEPLAY_MODE_IRRIGATION) {
        g->gameplayMode = GAMEPLAY_MODE_NORMAL;
    } else {
        g->gameplayMode = GAMEPLAY_MODE_IRRIGATION;
    }
}

void command_tileClicked(Garden *garden, int tileIndex, enum GameplayMode gameplayMode) {
    switch (gameplayMode) {

    case GAMEPLAY_MODE_NORMAL:
        garden->tileSelected = tileIndex;
        break;

    case GAMEPLAY_MODE_IRRIGATION:
        if (garden->tiles[tileIndex].hasPlanter && garden->tiles[tileIndex].planter.hasPlant) {
            plant_irrigate(&garden->tiles[tileIndex].planter.plant);
        }
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

    case COMMAND_ADD_PLANTER:
        command_addPlanter(&g->garden);
        break;

    case COMMAND_REMOVE_PLANTER:
        command_removePlanter(&g->garden);
        break;

    case COMMAND_ADD_PLANT:
        command_addPlant(&g->garden, &g->ui, cmd.args.plantType);
        break;

    case COMMAND_OPEN_PLANT_SELECTION:
        command_openPlantSelection(&g->ui, &g->garden);
        break;

    case COMMAND_REMOVE_PLANT:
        command_removePlant(&g->garden);
        break;

    case COMMAND_IRRIGATE:
        command_irrigate(&g->garden);
        break;

    case COMMAND_FEED:
        command_feed(&g->garden);
        break;

    case COMMAND_IRRIGATION_MODE:
        command_startIrrigationMode(g);
        break;

    case COMMAND_TILE_CLICKED:
        command_tileClicked(&g->garden, cmd.args.tileIndex, g->gameplayMode);

    case COMMAND_NONE:
        return false;
        break;
    }

    return true;
}
