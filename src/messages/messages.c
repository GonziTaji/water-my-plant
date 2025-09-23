#include "messages.h"
#include "../core/asset_manager.h"
#include "../entity/garden.h"
#include "../game/game.h"
#include "../input/input.h"
#include <assert.h>
#include <stdlib.h>

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

static void resetZoomView(Garden *garden) {
    garden->transform.scale = GARDEN_SCALE_INITIAL;
    garden_updateGardenOrigin(garden);
}

static void zoomView(Garden *garden, float amount) {
    float newScale
        = utils_clampf(GARDEN_SCALE_MIN, GARDEN_SCALE_MAX, garden->transform.scale + amount);

    if (newScale != garden->transform.scale) {
        garden->transform.scale = newScale;
        garden_updateGardenOrigin(garden);
    }
}

static void rotateView(Garden *garden) {
    garden->transform.rotation = utils_rotate(garden->transform.rotation, 1);
    garden_updateGardenOrigin(garden);
}

static void moveView(Garden *garden, Vector2 delta) {
    Vector2 *gardenTranslation = &garden->transform.translation;
    gardenTranslation->x -= delta.x;
    gardenTranslation->y -= delta.y;

    // Limits to clamp
    int gardenWidth = garden->tileGrid.cols * garden->tileGrid.tileWidth * garden->transform.scale;
    int gardenHeight
        = garden->tileGrid.rows * garden->tileGrid.tileHeight * garden->transform.scale;

    int minVisible = 4 * TILE_WIDTH * garden->transform.scale;

    int leftLimit = -gardenWidth + minVisible;
    int rightLimit = garden->screenSize->x - minVisible;

    int topLimit = -gardenHeight + minVisible;
    int bottomLimit = garden->screenSize->y - minVisible;

    gardenTranslation->x = utils_clampf(leftLimit, rightLimit, gardenTranslation->x);
    gardenTranslation->y = utils_clampf(topLimit, bottomLimit, gardenTranslation->y);
}

void rotateSelection(Garden *garden) {
    garden->selectionRotation++;

    if (garden->selectionRotation == ROTATION_COUNT) {
        garden->selectionRotation = 0;
    }
}

/// returns true if the planter could be added
static bool addPlanterToSelectedTile(Garden *garden, PlanterType planterType) {
    if (garden_hasPlanterSelected(garden)) {
        return false;
    }

    Vector2 dimensions = planter_getFootPrint(planterType, garden->selectionRotation);
    Vector2 origin = grid_getCoordsFromTileIndex(garden->tileGrid.cols, garden->tileSelected);
    Vector2 end = (Vector2){dimensions.x + origin.x, dimensions.y + origin.y};

    // zero coords
    for (int x = origin.x; x < end.x; x++) {
        for (int y = origin.y; y < end.y; y++) {
            // could be outside
            if (!grid_isValidCoords(garden->tileGrid.cols, garden->tileGrid.rows, x, y)) {
                return false;
            }

            int index
                = grid_getTileIndexFromCoords(garden->tileGrid.cols, garden->tileGrid.rows, x, y);

            if (garden->tiles[index].planterIndex != -1) {
                return false;
            }
        }
    }

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
            int tileIndex
                = grid_getTileIndexFromCoords(garden->tileGrid.cols, garden->tileGrid.rows, x, y);

            garden->tiles[tileIndex].planterIndex = planterIndex;
        }
    }

    return true;
}

static bool movePlanter(Garden *garden, int planterIndex, int destinationTileIndex) {
    Planter *planter = &garden->planters[garden->planterPickedUpIndex];

    Vector2 dimensions = planter_getFootPrint(planter->type, garden->selectionRotation);
    Vector2 coords = grid_getCoordsFromTileIndex(garden->tileGrid.cols, destinationTileIndex);
    Vector2 end = (Vector2){dimensions.x + coords.x, dimensions.y + coords.y};

    // zero coords
    for (int x = coords.x; x < end.x; x++) {
        for (int y = coords.y; y < end.y; y++) {
            // could be outside
            if (!grid_isValidCoords(garden->tileGrid.cols, garden->tileGrid.rows, x, y)) {
                return false;
            }

            int tileIndex
                = grid_getTileIndexFromCoords(garden->tileGrid.cols, garden->tileGrid.rows, x, y);

            int tilePlanterIndex = garden->tiles[tileIndex].planterIndex;

            if (tilePlanterIndex != -1 && tilePlanterIndex != planterIndex) {
                return false;
            }
        }
    }

    Rotation r = utils_rotate(garden->transform.rotation, planter->rotation);
    Vector2 oldDimensions = planter_getFootPrint(planter->type, r);
    Vector2 oldEnd = (Vector2){
        oldDimensions.x + planter->coords.x,
        oldDimensions.y + planter->coords.y,
    };

    for (int x = planter->coords.x; x < oldEnd.x; x++) {
        for (int y = planter->coords.y; y < oldEnd.y; y++) {
            int tileIndex
                = grid_getTileIndexFromCoords(garden->tileGrid.cols, garden->tileGrid.rows, x, y);

            garden->tiles[tileIndex].planterIndex = -1;
        }
    }

    planter->coords.x = coords.x;
    planter->coords.y = coords.y;
    planter->rotation = garden->selectionRotation;
    planter->size = dimensions;

    for (int x = planter->coords.x; x < end.x; x++) {
        for (int y = planter->coords.y; y < end.y; y++) {
            int tileIndex
                = grid_getTileIndexFromCoords(garden->tileGrid.cols, garden->tileGrid.rows, x, y);

            garden->tiles[tileIndex].planterIndex = planterIndex;
        }
    }

    return true;
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

            Rotation r = utils_rotate(garden->transform.rotation, planter->rotation);
            Vector2 oldDimensions = planter_getFootPrint(planter->type, r);
            Vector2 oldEnd = (Vector2){
                oldDimensions.x + planter->coords.x,
                oldDimensions.y + planter->coords.y,
            };

            for (int x = planter->coords.x; x < oldEnd.x; x++) {
                for (int y = planter->coords.y; y < oldEnd.y; y++) {
                    int tileIndex = grid_getTileIndexFromCoords(
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

static void addPlantToSelectedPlanter(Garden *garden, Vector2 worldMousePos, enum PlantType type) {
    Planter *planter = garden_getSelectedPlanter(garden);

    if (planter == NULL || planter->plantGrid.tileCount == 0) {
        return;
    }

    Vector2 planterOrigin = garden_getTileOrigin(garden, planter->coords);

    int plantIndex = planter_getPlantIndexFromWorldPos(
        planter, &garden->transform, planterOrigin, worldMousePos);

    Plant *plant = &planter->plants[plantIndex];

    if (!plant->exists) {
        planter_addPlant(planter, plantIndex, type);
    }
}

static void irrigateSelectedPlant(Garden *garden) {
    Planter *planter = garden_getSelectedPlanter(garden);

    if (planter == NULL) {
        return;
    }

    Vector2 tileCoords = grid_getCoordsFromTileIndex(garden->tileGrid.cols, garden->tileSelected);

    int plantIndex = planter_getPlantIndexFromGridCoords(planter, tileCoords);
    Plant *plant = &planter->plants[plantIndex];

    if (plant->exists) {
        plant_irrigate(plant);
    }
}

static void feedSelectedPlant(Garden *garden) {
    Planter *planter = garden_getSelectedPlanter(garden);

    if (planter == NULL) {
        return;
    }

    Vector2 tileCoords = grid_getCoordsFromTileIndex(garden->tileGrid.cols, garden->tileSelected);

    int plantIndex = planter_getPlantIndexFromGridCoords(planter, tileCoords);
    Plant *plant = &planter->plants[plantIndex];

    if (plant->exists) {
        plant_feed(plant);
    }
}

static void changeTool(Game *g, enum GardeningTool tool) {
    g->toolSelected = tool;
    g->ui.toolSelectionButtonPannel.activeButtonIndex = g->toolSelected;

    // resets
    if (g->garden.planterPickedUpIndex != -1) {
        g->garden.planterPickedUpIndex = -1;
    }

    ui_syncToolVariantPanelToSelection(&g->ui, tool, g->toolVariantsSelection[tool]);
}

static void pickupOrSetPlanter(Garden *garden) {
    if (garden->planterPickedUpIndex == -1) {
        Planter *planter = garden_getSelectedPlanter(garden);

        if (planter == NULL) {
            return;
        }

        if (!planter->exists) {
            return;
        }

        garden->planterPickedUpIndex = garden->tiles[garden->tileSelected].planterIndex;
        garden->selectionRotation = utils_rotate(planter->rotation, garden->transform.rotation);
    } else {
        bool added = movePlanter(garden, garden->planterPickedUpIndex, garden->tileSelected);

        if (added) {
            garden->planterPickedUpIndex = -1;
        }
    }
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
        addPlantToSelectedPlanter(&game->garden,
            game->input.worldMousePos,
            game->toolVariantsSelection[game->toolSelected]);
        break;

    case GARDENING_TOOL_TRASH_BIN:
        removeFromTile(&game->garden, game->input.worldMousePos);
        break;

    case GARDENING_TOOL_NONE:
        pickupOrSetPlanter(&game->garden);
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

    case MESSAGE_CMD_VIEW_MOVE:
        moveView(&g->garden, msg.args.vector);
        break;

    case MESSAGE_CMD_VIEW_ROTATE:
        rotateView(&g->garden);
        break;

    case MESSAGE_CMD_VIEW_ZOOM_UP:
        zoomView(&g->garden, GARDEN_SCALE_STEP);
        break;

    case MESSAGE_CMD_VIEW_ZOOM_DOWN:
        zoomView(&g->garden, -GARDEN_SCALE_STEP);
        break;

    case MESSAGE_CMD_VIEW_ZOOM_RESET:
        resetZoomView(&g->garden);
        break;

    case MESSAGE_CMD_TOOL_VARIANT_ROTATE:
        rotateSelection(&g->garden);
        break;

    case MESSAGE_EV_UI_CLICKED:
        // fallback
        break;

    case MESSAGE_NONE:
        return false;
    }

    return true;
}
