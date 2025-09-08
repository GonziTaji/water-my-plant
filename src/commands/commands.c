#include "commands.h"
#include "../core/asset_manager.h"
#include "../entity/garden.h"
#include "../game/game.h"
#include "../utils/utils.h"
#include <assert.h>

void command_addPlanter(Garden *garden, PlanterType planterType) {
    if (garden_hasPlanterSelected(garden)) {
        return;
    }

    Rotation rotation = utils_rotate(garden->rotation, garden->selectionRotation);
    Vector2 dimensions = planter_getDimensions(planterType, rotation);
    Vector2 origin = garden_getTileCoordsFromIndex(garden, garden->tileSelected);
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

            int index = garden_getTileIndexFromCoords(garden, x, y);

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

        Planter *p = &garden->planters[planterIndex];

        planter_init(p, planterType, origin, garden->selectionRotation);

        for (int x = p->bounds.x; x < end.x; x++) {
            for (int y = p->bounds.y; y < end.y; y++) {
                int tileIndex = garden_getTileIndexFromCoords(garden, x, y);

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
                planter->bounds.x + planter->bounds.width,
                planter->bounds.y + planter->bounds.height,
            };

            for (int x = planter->bounds.x; x < end.x; x++) {
                for (int y = planter->bounds.y; y < end.y; y++) {
                    int tileIndex = garden_getTileIndexFromCoords(garden, x, y);
                    garden->tiles[tileIndex].planterIndex = -1;
                }
            }
        }
    }
}

void command_toolVariantSelected(Game *g, int variant) {
    g->toolVariantsSelection[g->toolSelected] = variant;
    ui_syncToolVariantPanelToSelection(&g->ui, g->toolSelected, variant);
}

void command_toolVariantSelectNext(Game *g) {
    int nextVariant = g->toolVariantsSelection[g->toolSelected] + 1;
    // change to a utils function to get the variant count based on tool?
    if (nextVariant == uiButtonGrid_getButtonsCount(&g->ui.toolVariantButtonPannel)) {
        nextVariant = 0;
    }

    command_toolVariantSelected(g, nextVariant);
}

void command_addPlant(Game *g) {
    Planter *planter = garden_getSelectedPlanter(&g->garden);

    if (!garden_hasPlanterSelected(&g->garden) || planter->hasPlant) {
        return;
    }

    planter_addPlant(planter, g->toolVariantsSelection[g->toolSelected]);
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

    g->ui.toolSelectionButtonPannel.activeButtonIndex = g->toolSelected;

    ui_syncToolVariantPanelToSelection(&g->ui, tool, g->toolVariantsSelection[tool]);
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
        command_addPlanter(&game->garden, game->toolVariantsSelection[game->toolSelected]);
        break;

    case GARDENING_TOOL_PLANT_CUTTING:
        command_addPlant(game);
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
    g->ui.speedSelectionButtonPannel.activeButtonIndex = newSpeed;
}

/// returns `true` if a command was executed (cmd.type is not "COMMAND_NONE"
bool command_dispatchCommand(Command cmd, Game *g) {
    switch (cmd.type) {
    case COMMAND_TILE_CLICKED:
        command_tileClicked(g, cmd.args.selection);
        break;

    case COMMAND_TOOL_SELECTED:
        command_changeTool(g, cmd.args.selection);
        break;

    case COMMAND_TOOL_VARIANT_SELECTED:
        command_toolVariantSelected(g, cmd.args.selection);
        break;

    case COMMAND_TOOL_VARIANT_SELECT_NEXT:
        command_toolVariantSelectNext(g);
        break;

    case COMMAND_GAMEPLAY_CHANGE_SPEED:
        command_changeGameplaySpeed(g, cmd.args.selection);
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
