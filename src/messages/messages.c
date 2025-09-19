#include "messages.h"
#include "../core/asset_manager.h"
#include "../entity/garden.h"
#include "../game/game.h"
#include "../input/input.h"
#include "../utils/utils.h"
#include <assert.h>

// try to name the functions following this conventions:
//
// - Events:
// Use "on" as prefix and then the event that is being handled in past tense. Put
// the verb at the end: "onTargetAction"
// i.e. onTileClicked, onPlantDied, onDayEnded
//
// Commands:
// Prefix the name with the action in present tense and then the target
// i.e. tileSoil, feedSelectedPlant, changeTool
//

static void addPlanterToSelectedTile(Garden *garden, PlanterType planterType) {
    if (garden_hasPlanterSelected(garden)) {
        return;
    }

    Vector2 dimensions = planter_getFootPrint(planterType, garden->selectionRotation);
    Vector2 origin = utils_grid_getCoordsFromTileIndex(garden->tileGrid.cols, garden->tileSelected);
    Vector2 end = (Vector2){dimensions.x + origin.x, dimensions.y + origin.y};

    bool planterFits = true;

    // zero coords
    for (int x = origin.x; x < end.x; x++) {
        for (int y = origin.y; y < end.y; y++) {
            // could be outside
            if (!utils_grid_isValidCoords(garden->tileGrid.cols, garden->tileGrid.rows, x, y)) {
                planterFits = false;
                break;
            }

            int index = utils_grid_getTileIndexFromCoords(
                garden->tileGrid.cols, garden->tileGrid.rows, x, y);

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
            if (!garden->planters[planterIndex].exists) {
                break;
            }
        }

        Planter *p = &garden->planters[planterIndex];

        planter_init(p, planterType, origin, garden->selectionRotation, garden->tileGrid.tileWidth);

        for (int x = p->coords.x; x < end.x; x++) {
            for (int y = p->coords.y; y < end.y; y++) {
                int tileIndex = utils_grid_getTileIndexFromCoords(
                    garden->tileGrid.cols, garden->tileGrid.rows, x, y);

                garden->tiles[tileIndex].planterIndex = planterIndex;
            }
        }
    }
}

static void removeFromTile(Garden *garden, Vector2 worldMousePos) {
    if (!garden_hasPlanterSelected(garden)) {
        return;
    }

    int planterIndex = garden->tiles[garden->tileSelected].planterIndex;

    if (planterIndex == -1) {
        // nothing to do
        return;
    }

    for (int i = 0; i < garden->tileGrid.tileCount; i++) {
        if (garden->tiles[i].planterIndex == planterIndex) {
            garden->tiles[i].planterIndex = -1;
        }
    }

    Planter *planter = &garden->planters[planterIndex];
    if (planter->exists == true) {
        Vector2 planterOrigin = garden_getTileOrigin(garden, planter->coords);

        int plantIndex = planter_getPlantIndexFromWorldPos(
            planter, &garden->transform, planterOrigin, worldMousePos);

        Plant *plant = &planter->plants[plantIndex];

        if (plant->exists) {
            plant->exists = false;
        } else {
            // TODO: do something if clicked on planter with plants, but in a empty plant space?
            planter->exists = false;

            Vector2 end = (Vector2){
                planter->coords.x + planter->size.x,
                planter->coords.y + planter->size.y,
            };

            for (int x = planter->coords.x; x < end.x; x++) {
                for (int y = planter->coords.y; y < end.y; y++) {
                    int tileIndex = utils_grid_getTileIndexFromCoords(
                        garden->tileGrid.cols, garden->tileGrid.rows, x, y);
                    garden->tiles[tileIndex].planterIndex = -1;
                }
            }
        }
    }
}

static void selectToolVariant(Game *g, int variant) {
    g->toolVariantsSelection[g->toolSelected] = variant;
    ui_syncToolVariantPanelToSelection(&g->ui, g->toolSelected, variant);
}

static void selectNextToolVariant(Game *g) {
    int nextVariant = g->toolVariantsSelection[g->toolSelected] + 1;
    // change to a utils function to get the variant count based on tool?
    if (nextVariant == uiButtonGrid_getButtonsCount(&g->ui.toolVariantButtonPannel)) {
        nextVariant = 0;
    }

    selectToolVariant(g, nextVariant);
}

static void addPlantToSelectedPlanter(Garden *garden, InputManager *input, enum PlantType type) {
    if (!garden_hasPlanterSelected(garden)) {
        return;
    }

    Planter *planter = garden_getSelectedPlanter(garden);

    if (planter->plantGrid.tileCount == 0) {
        return;
    }

    Vector2 planterOrigin = garden_getTileOrigin(garden, planter->coords);

    int plantIndex = planter_getPlantIndexFromWorldPos(
        planter, &garden->transform, planterOrigin, input->worldMousePos);

    Plant *plant = &planter->plants[plantIndex];

    if (!plant->exists) {
        planter_addPlant(planter, plantIndex, type);
    }
}

static void irrigateSelectedPlant(Garden *garden) {
    if (!garden_hasPlanterSelected(garden)) {
        return;
    }

    Vector2 tileCoords
        = utils_grid_getCoordsFromTileIndex(garden->tileGrid.cols, garden->tileSelected);

    Planter *planter = garden_getSelectedPlanter(garden);

    int plantIndex = planter_getPlantIndexFromGridCoords(planter, tileCoords);
    Plant *plant = &planter->plants[plantIndex];

    if (plant->exists) {
        plant_irrigate(plant);
    }
}

static void feedSelectedPlant(Garden *garden) {
    if (!garden_hasPlanterSelected(garden)) {
        return;
    }

    Vector2 tileCoords
        = utils_grid_getCoordsFromTileIndex(garden->tileGrid.cols, garden->tileSelected);

    Planter *planter = garden_getSelectedPlanter(garden);

    int plantIndex = planter_getPlantIndexFromGridCoords(planter, tileCoords);
    Plant *plant = &planter->plants[plantIndex];

    if (plant->exists) {
        plant_feed(plant);
    }
}

static void changeTool(Game *g, enum GardeningTool tool) {
    g->toolSelected = tool;
    g->ui.toolSelectionButtonPannel.activeButtonIndex = g->toolSelected;

    ui_syncToolVariantPanelToSelection(&g->ui, tool, g->toolVariantsSelection[tool]);
}

static void onTileClicked(Game *game, int tileIndex) {
    game->garden.tileSelected = tileIndex;

    switch (game->toolSelected) {
    case GARDENING_TOOL_IRRIGATOR:
        irrigateSelectedPlant(&game->garden);
        break;

    case GARDENING_TOOL_NUTRIENTS:
        feedSelectedPlant(&game->garden);
        break;

    case GARDENING_TOOL_PLANTER:
        addPlanterToSelectedTile(&game->garden, game->toolVariantsSelection[game->toolSelected]);
        break;

    case GARDENING_TOOL_PLANT_CUTTING:
        addPlantToSelectedPlanter(
            &game->garden, &game->input, game->toolVariantsSelection[game->toolSelected]);
        break;

    case GARDENING_TOOL_TRASH_BIN:
        removeFromTile(&game->garden, game->input.worldMousePos);
        break;

    case GARDENING_TOOL_NONE:
        game->toolSelected = GARDENING_TOOL_NONE;
        break;

    case GARDENING_TOOL_COUNT:
        assert(false);
        break;
    }
}

static void changeGameplaySpeed(Game *g, GameplaySpeed newSpeed) {
    g->gameplaySpeed = newSpeed;
    g->ui.speedSelectionButtonPannel.activeButtonIndex = newSpeed;
}

/// returns `true` if a the command executed blocks further messages for this frame
bool messages_dispatchMessage(Message msg, Game *g) {
    switch (msg.type) {
    case MESSAGE_EV_TILE_CLICKED:
        onTileClicked(g, msg.args.selection);
        break;

    case MESSAGE_CMD_TOOL_SELECT:
        changeTool(g, msg.args.selection);
        break;

    case MESSAGE_CMD_TOOL_VARIANT_SELECT:
        selectToolVariant(g, msg.args.selection);
        break;

    case MESSAGE_CMD_TOOL_VARIANT_SELECT_NEXT:
        selectNextToolVariant(g);
        break;

    case MESSAGE_CMD_GAMEPLAY_SPEED_CHANGE:
        changeGameplaySpeed(g, msg.args.selection);
        break;

    case MESSAGE_EV_UI_CLICKED:
        // fallback
        break;

    case MESSAGE_NONE:
        return false;
        break;
    }

    return true;
}
