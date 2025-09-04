#include "commands.h"
#include "../core/asset_manager.h"
#include "../entity/garden.h"
#include "../game/game.h"
#include <assert.h>

void command_addPlanter(Garden *garden, PlanterType planterType) {
    if (garden_hasPlanterSelected(garden)) {
        return;
    }

    Vector2 dimensions = planter_getDimensions(planterType);
    Vector2 origin = garden_getPlanterCoordsFromIndex(garden, garden->tileSelected);
    Vector2 end = (Vector2){dimensions.x + origin.x, dimensions.y + origin.y};

    bool planterFits = true;

    // zero coords
    for (int x = origin.x; x < end.x; x++) {
        for (int y = origin.y; y < end.y; y++) {
            // could be outside
            if (!garden_isValidGridCoords(garden, x, y)) {
                planterFits = false;
                break;
            }

            int index = garden_getPlanterIndexFromCoords(garden, x, y);

            if (garden->tiles[index].planterIndex != -1) {
                planterFits = false;
                break;
            }
        }

        if (planterFits == false) {
            break;
        }
    }

    if (planterFits) {
        int planterIndex;
        for (planterIndex = 0; planterIndex < GARDEN_MAX_TILES; planterIndex++) {
            if (!garden->planters[planterIndex].alive) {
                break;
            }
        }

        planter_init(&garden->planters[planterIndex], planterType, origin);

        for (int x = origin.x; x < end.x; x++) {
            for (int y = origin.y; y < end.y; y++) {
                int tileIndex = garden_getPlanterIndexFromCoords(garden, x, y);

                garden->tiles[tileIndex].planterIndex = planterIndex;
            }
        }
    }
}

void command_removeFromTile(Garden *garden) {
    if (!garden_hasPlanterSelected(garden)) {
        return;
    }

    int planterIndex = garden->tiles[garden->tileSelected].planterIndex;

    if (planterIndex == -1) {
        // nothing to do
        return;
    }

    Planter *planter = &garden->planters[planterIndex];

    if (planter->alive == true) {
        if (planter->hasPlant) {
            planter_removePlant(planter);
        } else {
            planter->alive = false;

            Vector2 end = (Vector2){
                planter->origin.x + planter->dimensions.x,
                planter->origin.y + planter->dimensions.y,
            };

            for (int x = planter->origin.x; x < end.x; x++) {
                for (int y = planter->origin.y; y < end.y; y++) {
                    int tileIndex = garden_getPlanterIndexFromCoords(garden, x, y);
                    garden->tiles[tileIndex].planterIndex = -1;
                }
            }
        }
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
            if (plantGrid->buttons[i].command.args.plantTypeSelected == type) {
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
            type = ui->plantSelectionButtonPannel.buttons[i].command.args.plantTypeSelected;
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

void command_tileClicked(Game *game, int tileIndex) {
    game->garden.tileSelected = tileIndex;

    switch (game->toolSelected) {
    case GARDENING_TOOL_IRRIGATOR:
        command_irrigate(&game->garden);
        break;

    case GARDENING_TOOL_NUTRIENTS:
        command_feed(&game->garden);
        break;

    case GARDENING_TOOL_PLANTER:
        command_addPlanter(&game->garden, game->planterTypeSelected);
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
        command_tileClicked(g, cmd.args.tileIndex);
        break;

    case COMMAND_TOOL_SELECTED:
        command_changeTool(g, cmd.args.tool);
        break;

    case COMMAND_PLANT_TYPE_SELECTED:
        command_changePlantSelected(g, cmd.args.plantTypeSelected);
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
