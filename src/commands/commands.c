#include "commands.h"
#include "../core/asset_manager.h"
#include "../entity/garden.h"
#include "../game/game.h"
#include <assert.h>

void command_addPlanter(Garden *garden) {
    if (garden->tileSelected == -1 && garden->tiles[garden->tileSelected].hasPlanter) {
        return;
    }

    garden->tiles[garden->tileSelected].hasPlanter = true;
    planter_init(garden_getSelectedPlanter(garden));
}

void command_removeFromTile(Garden *garden) {
    if (!garden_hasPlanterSelected(garden)) {
        return;
    }

    Planter *planter = garden_getSelectedPlanter(garden);

    if (!planter->hasPlant) {
        garden->tiles[garden->tileSelected].hasPlanter = false;
        return;
    } else {
        planter_removePlant(planter);
    }
}

void command_changePlantSelected(Game *g, enum PlantType type) {
    if (g->toolSelected != GARDENING_TOOL_PLANT_CUTTING) {
        return;
    }

    if (g->plantTypeSelected == type) {
        return;
    }

    g->plantTypeSelected = type;

    UIButtonGrid *plantGrid = &g->ui.plantSelectionButtonPannel;

    for (int i = 0; i < plantGrid->buttonsCount; i++) {
        if (plantGrid->buttons[i].command.type == COMMAND_PLANT_TYPE_SELECTED) {
            if (plantGrid->buttons[i].command.args.plantType == type) {
                plantGrid->buttons[i].status = BUTTON_STATUS_ACTIVE;
            } else {
                plantGrid->buttons[i].status = BUTTON_STATUS_NORMAL;
            }
        }
    }
}

void command_changePlantSelectNext(Game *g) {
    int plantSelected = g->plantTypeSelected + 1;
    if (plantSelected == PLANT_TYPE_COUNT) {
        plantSelected = 0;
    }

    command_changePlantSelected(g, plantSelected);
}

void command_addPlant(Garden *garden, UI *ui) {
    Planter *planter = garden_getSelectedPlanter(garden);

    if (!garden_hasPlanterSelected(garden) || planter->hasPlant) {
        return;
    }

    // save in memory somewhere?
    enum PlantType type;
    for (int i = 0; i < ui->plantSelectionButtonPannel.buttonsCount; i++) {
        if (ui->plantSelectionButtonPannel.buttons[i].status == BUTTON_STATUS_ACTIVE) {
            type = ui->plantSelectionButtonPannel.buttons[i].command.args.plantType;
        }
    }

    planter_addPlant(planter, type);
}

void command_irrigate(Garden *garden) {
    Planter *planter = garden_getSelectedPlanter(garden);

    if (!garden_hasPlanterSelected(garden) || !planter->hasPlant) {
        return;
    }

    plant_irrigate(&planter->plant);
}

void command_feed(Garden *garden) {
    Planter *planter = garden_getSelectedPlanter(garden);

    if (!garden_hasPlanterSelected(garden) || !planter->hasPlant) {
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

    UIButtonGrid *toolGrid = &g->ui.toolSelectionButtonPannel;

    for (int i = 0; i < toolGrid->buttonsCount; i++) {
        if (toolGrid->buttons[i].command.type != COMMAND_TOOL_SELECTED) {
            continue;
        }

        if (toolGrid->buttons[i].command.args.tool == g->toolSelected) {
            toolGrid->buttons[i].status = BUTTON_STATUS_ACTIVE;
        } else {
            toolGrid->buttons[i].status = BUTTON_STATUS_NORMAL;
        }
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
        command_addPlant(&game->garden, &game->ui);
        break;

    case GARDENING_TOOL_TRASH_BIN:
        command_removeFromTile(&game->garden);
        break;

    case GARDENING_TOOL_NONE:
        game->toolSelected = GARDENING_TOOL_NONE;
        break;

    case GARDENING_TOOL_COUNT:
        assert(false);
        break;
    }
}

void command_changeGameplaySpeed(Game *g, GameplaySpeed newSpeed) {
    g->gameplaySpeed = newSpeed;
    UIButtonGrid *speedGrid = &g->ui.speedSelectionButtonPannel;

    for (int i = 0; i < speedGrid->buttonsCount; i++) {
        if (speedGrid->buttons[i].command.type == COMMAND_GAMEPLAY_CHANGE_SPEED) {
            if (speedGrid->buttons[i].command.args.gameplaySpeed == newSpeed) {
                speedGrid->buttons[i].status = BUTTON_STATUS_ACTIVE;
            } else {
                speedGrid->buttons[i].status = BUTTON_STATUS_NORMAL;
            }
        }
    }
}

/// returns `true` if a command was executed (cmd.type is not "COMMAND_NONE"
bool command_dispatchCommand(Command cmd, Game *g) {
    switch (cmd.type) {
    case COMMAND_TILE_CLICKED:
        command_tileClicked(g, cmd.args.tileIndex, g->toolSelected);
        break;

    case COMMAND_TOOL_SELECTED:
        command_changeTool(g, cmd.args.tool);
        break;

    case COMMAND_PLANT_TYPE_SELECTED:
        command_changePlantSelected(g, cmd.args.plantType);
        break;

    case COMMAND_PLANT_TYPE_SELECT_NEXT:
        command_changePlantSelectNext(g);
        break;

    case COMMAND_GAMEPLAY_CHANGE_SPEED:
        command_changeGameplaySpeed(g, cmd.args.gameplaySpeed);
        break;

    case COMMAND_UI_CLICKED:
        // fallback
        break;

    case COMMAND_NONE:
        return false;
        break;
    }

    return true;
}
