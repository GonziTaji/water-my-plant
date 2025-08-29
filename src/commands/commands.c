#include "../core/asset_manager.h"
#include "../entity/garden.h"
#include "../game/game.h"
#include "../ui/input_manager.h"

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

void command_requestPlantTypeToAdd(Garden *garden, UI *ui, InputManager *input) {
    Planter *p = garden_getSelectedPlanter(garden);

    if (!garden_hasPlanterSelected(garden) || p->hasPlant) {
        return;
    }

    ui->showPlantSelection = true;

    Vector2 buttonPannelPos = {
        input->worldMousePos.x + 20,
        input->worldMousePos.y + 20,
    };

    buttonPannel_translate(&ui->plantSelectionButtonPannel, buttonPannelPos);
}

void command_addPlant(Garden *garden, UI *ui, enum PlantType type) {
    Planter *planter = garden_getSelectedPlanter(garden);

    if (!garden_hasPlanterSelected(garden) || planter->hasPlant) {
        return;
    }

    planter_addPlant(planter, type);
    ui->showPlantSelection = false;
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
        command_requestPlantTypeToAdd(&game->garden, &game->ui, &game->input);
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
