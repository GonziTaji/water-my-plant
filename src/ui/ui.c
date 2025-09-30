#include "ui.h"
#include "../core/asset_manager.h"
#include "button.h"
#include "raylib.h"
#include "ui_text_box.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

Rectangle getToolVariantSpriteSourceRec(enum GardeningTool tool, int toolVariant) {
    switch (tool) {
    case GARDENING_TOOL_TRASH_BIN:
    case GARDENING_TOOL_NONE:
    case GARDENING_TOOL_IRRIGATOR:
    case GARDENING_TOOL_NUTRIENTS:
    case GARDENING_TOOL_COUNT:
        return (Rectangle){};

    case GARDENING_TOOL_PLANTER:
        return planter_getSpriteSourceRec(toolVariant, ROTATION_0, ROTATION_0);

    case GARDENING_TOOL_PLANT_CUTTING:
        return plant_getSpriteSourceRect(toolVariant, 100);
    }

    return (Rectangle){};
}

void ui_syncToolVariantPanelToSelection(
    UI *ui, enum GardeningTool toolSelected, int toolVariantSelected) {

    UIButtonGrid *variantGrid = &ui->toolVariantButtonPannel;

    int maxVariants;
    Texture2D variantTexture;

    switch (toolSelected) {
    case GARDENING_TOOL_TRASH_BIN:
    case GARDENING_TOOL_NONE:
    case GARDENING_TOOL_IRRIGATOR:
    case GARDENING_TOOL_NUTRIENTS:
    case GARDENING_TOOL_COUNT:
        ui->showToolVariantPanel = false;
        return;

    case GARDENING_TOOL_PLANTER:
        maxVariants = PLANTER_TYPE_COUNT;
        variantTexture = planterAtlas;
        break;

    case GARDENING_TOOL_PLANT_CUTTING:
        maxVariants = PLANT_TYPE_COUNT;
        variantTexture = plantAtlas;
        break;
    }

    ui->showToolVariantPanel = true;

    variantGrid->cols = maxVariants;
    variantGrid->rows = 1;

    for (int i = 0; i < maxVariants; i++) {
        variantGrid->buttons[i].content = (ButtonContent){
            .icon = {variantTexture, getToolVariantSpriteSourceRec(toolSelected, i)},
        };

        if (i == toolVariantSelected) {
            variantGrid->activeButtonIndex = i;
        }
    }

    UIButtonGrid *toolButtonGrid = &ui->toolSelectionButtonPannel;
    float toolVariantGridWidth = uiButtonGrid_getFullWidth(variantGrid);
    float toolVariantGridHeight = uiButtonGrid_getFullHeight(variantGrid);
    float toolGridWidth = uiButtonGrid_getFullWidth(toolButtonGrid);

    variantGrid->origin = (Vector2i){
        toolButtonGrid->origin.x + (toolGridWidth / 2) - (toolVariantGridWidth / 2),
        toolButtonGrid->origin.y - toolVariantGridHeight - 20,
    };
}

void ui_init(UI *ui, Vector2 *screenSize, GameplaySpeed gameplaySpeed) {
    HideCursor();

    ui->showToolVariantPanel = false;

    UIButtonGrid *toolGrid = &ui->toolSelectionButtonPannel;

    uiButtonGrid_init(toolGrid,
        GARDENING_TOOL_COUNT,
        1,
        (Vector2i){160, 80},
        (Vector2i){4, 4},
        (Vector2i){4, 4},
        (Vector2i){0, 0});

    float toolGridWidth = uiButtonGrid_getFullWidth(toolGrid);
    float toolGridHeight = uiButtonGrid_getFullHeight(toolGrid);

    toolGrid->origin = (Vector2i){
        (screenSize->x - toolGridWidth) / 2,
        screenSize->y - (toolGridHeight + 20),
    };

    ButtonContent bcontent = (ButtonContent){.label = ""};

    int buttonsCount = uiButtonGrid_getButtonsCount(toolGrid);

    for (int i = 0; i < buttonsCount; i++) {
        switch (i) {
        case GARDENING_TOOL_NONE:
            bcontent.label = "Select [Esc]";
            break;

        case GARDENING_TOOL_IRRIGATOR:
            bcontent.label = "[W]ater";
            break;

        case GARDENING_TOOL_NUTRIENTS:
            bcontent.label = "[F]eed";
            break;

        case GARDENING_TOOL_PLANTER:
            bcontent.label = "Add planter [M]";
            break;

        case GARDENING_TOOL_PLANT_CUTTING:
            bcontent.label = "[A]dd Plant";
            break;

        case GARDENING_TOOL_TRASH_BIN:
            bcontent.label = "[R]emove";
            break;

        case GARDENING_TOOL_COUNT:
            continue;
        }

        toolGrid->buttons[i] = (UIButton){
            .type = BUTTON_TYPE_TEXT_LABEL,
            .content = bcontent,
            .command = (Message){MESSAGE_CMD_TOOL_SELECT, {.selection = i}},
        };
    }

    uiButtonGrid_init(&ui->speedSelectionButtonPannel,
        GAMEPLAY_SPEED_COUNT,
        1,
        (Vector2i){80, 30},
        (Vector2i){4, 4},
        (Vector2i){4, 4},
        (Vector2i){100, 10});

    ui->speedSelectionButtonPannel.activeButtonIndex = gameplaySpeed;

    for (int i = 0; i < GAMEPLAY_SPEED_COUNT; i++) {
        ui->speedSelectionButtonPannel.buttons[i] = (UIButton){
            .type = BUTTON_TYPE_TEXT_LABEL,
            .content = {.label = ">"},
            .command = (Message){MESSAGE_CMD_GAMEPLAY_SPEED_CHANGE, {.selection = i}},
        };

        if (i == gameplaySpeed) {
            ui->speedSelectionButtonPannel.activeButtonIndex = i;
        }
    }

    // If the UI is always initialized with no tool selected, we don't need this
    // ui_syncToolVariantPanelToSelection()

    uiButtonGrid_init(&ui->toolVariantButtonPannel,
        0,
        0,
        (Vector2i){160, 80},
        (Vector2i){4, 4},
        (Vector2i){4, 4},
        (Vector2i){0, 10});

    // commands for tool variation are always the same, at least for now
    for (int i = 0; i < UI_BUTTON_PANNEL_MAX_BUTTONS; i++) {
        ui->toolVariantButtonPannel.buttons[i].type = BUTTON_TYPE_SPRITE;
        ui->toolVariantButtonPannel.buttons[i].command = (Message){
            MESSAGE_CMD_TOOL_VARIANT_SELECT,
            {.selection = i},
        };
    }
}

Message ui_processInput(UI *ui, InputManager *input, enum GardeningTool tool) {
    Message msg;

    if (ui->showToolVariantPanel) {
        msg = uiButtonGrid_processInput(&ui->toolVariantButtonPannel, input);

        if (msg.type != MESSAGE_NONE) {
            return msg;
        }
    }

    msg = uiButtonGrid_processInput(&ui->toolSelectionButtonPannel, input);

    if (msg.type != MESSAGE_NONE) {
        return msg;
    }

    msg = uiButtonGrid_processInput(&ui->speedSelectionButtonPannel, input);

    if (msg.type != MESSAGE_NONE) {
        return msg;
    }

    return (Message){MESSAGE_NONE};
}

void ui_draw(UI *ui,
    InputManager *input,
    Vector2 *screenSize,
    Garden *garden,
    enum GardeningTool toolSelected,
    float gameplayTime) {

    int bpFontSize = uiFont.baseSize;
    uiButtonGrid_draw(&ui->toolSelectionButtonPannel, bpFontSize);
    uiButtonGrid_draw(&ui->speedSelectionButtonPannel, bpFontSize);

    if (ui->showToolVariantPanel) {
        uiButtonGrid_draw(&ui->toolVariantButtonPannel, bpFontSize);
    }

    int tileIndex = -1;

    if (garden->tileHovered != -1) {
        tileIndex = garden->tileHovered;
    } else if (garden->tileSelected != -1) {
        tileIndex = garden->tileHovered;
    }

    int fontSize = uiFont.baseSize;
    int tileInfoRecWidth = 250;

    char buffer[64];

    if (tileIndex != -1) {
        Vector2 tileCoords = grid_getCoordsFromTileIndex(GARDEN_COLS, tileIndex);

        snprintf(buffer,
            sizeof(buffer),
            "Tile selected: X, Y = %d, %d",
            (int)tileCoords.x,
            (int)tileCoords.y);

        DrawTextEx(uiFont, buffer, (Vector2){100, 300}, fontSize, 0, WHITE);

        Vector2 distanceToLeftVertice = grid_getDistanceFromFarthestTile(SCENE_TRANSFORM.rotation,
            tileCoords.x,
            tileCoords.y,
            GARDEN_COLS,
            GARDEN_ROWS);

        snprintf(buffer,
            sizeof(buffer),
            "distance to left vertice (X, Y) = %d, %d",
            (int)distanceToLeftVertice.x,
            (int)distanceToLeftVertice.y);

        DrawTextEx(uiFont, buffer, (Vector2){100, 330}, fontSize, 0, WHITE);

        int lightLevel = garden->tiles[tileIndex].lightLevel;
        int planterIndex = garden->tiles[tileIndex].planterIndex;
        Planter *planter = &garden->planters[planterIndex];

        Vector2 offset = {-20, 20};
        Rectangle tbBounds = {
            offset.x + screenSize->x - tileInfoRecWidth,
            offset.y,
            tileInfoRecWidth,
            screenSize->y - (offset.y * 2),
        };

        UITextBox tb;
        uiTextBox_init(&tb, uiFont, fontSize, tbBounds, (Vector2){20, 20});

        snprintf(buffer, sizeof(buffer), "Light level: %d", lightLevel);

        uiTextBox_drawTextLine(&tb, buffer, BLACK);
        uiTextBox_drawTextLine(&tb, "", BLACK); // spacing

        if (planterIndex != -1 && planter->exists) {
            Vector2 planterOrigin = garden_getTileOrigin(garden, planter->coords);

            int plantIndex
                = planter_getPlantIndexFromWorldPos(planter, planterOrigin, input->worldMousePos);

            const Plant *plant = &planter->plants[plantIndex];

            if (plantIndex != -1 && plant->exists) {
                uiTextBox_drawTextLine(&tb, "Plant info:", BLACK);
                tb.cursorPosition.y += 5; // spacing

                struct {
                    const char *label;
                    const char *value;
                } infoArr[] = {
                    {"Scientific name", plantDefinitions[plant->type].scientificName},
                    {"Name", plantDefinitions[plant->type].name},
                };

                int infoLinesCount = 2;

                tb.cursorPosition.x += 20;
                for (int i = 0; i < infoLinesCount; i++) {
                    snprintf(buffer, sizeof(buffer), "%s:", infoArr[i].label);
                    uiTextBox_drawTextLine(&tb, buffer, BLACK);

                    snprintf(buffer, sizeof(buffer), "- %s", infoArr[i].value);
                    uiTextBox_drawTextLine(&tb, buffer, BLACK);
                }
                tb.cursorPosition.x -= 20;

                tb.cursorPosition.y += 16; // spacing
                uiTextBox_drawTextLine(&tb, "Plant Stats:", BLACK);
                tb.cursorPosition.y += 5; // spacing

                struct {
                    const char *label;
                    float value;
                } stats[] = {
                    {"Soil Water", plant->mediumHydration},
                    {"Soil Nutrients", plant->mediumNutrition},
                    {"Water", plant->hydration},
                    {"Nutrients", plant->nutrition},
                    {"Health", plant->health},
                };

                int statsCount = 5;

                for (int i = 0; i < statsCount; i++) {
                    int statBarWitdh = 200;
                    // TODO: fix magic numbers
                    int level = plant_getStatLevel(stats[i].value);
                    int amountByLevel = (100.0f / PLANT_STATUS_LEVEL_COUNT);
                    int baseAmount = amountByLevel * level;
                    int restOfLevel = stats[i].value - baseAmount;
                    int internalWidth = (statBarWitdh / amountByLevel) * restOfLevel;

                    DrawRectangleV(
                        tb.cursorPosition, (Vector2){internalWidth, fontSize}, LIGHTGRAY);
                    DrawRectangleLines(
                        tb.cursorPosition.x, tb.cursorPosition.y, statBarWitdh, fontSize, WHITE);

                    tb.cursorPosition.x += 20;
                    snprintf(buffer, sizeof(buffer), "%d - %s", level, stats[i].label);
                    uiTextBox_drawTextLine(&tb, buffer, BLACK);
                    tb.cursorPosition.x -= 20;

                    tb.cursorPosition.y += 5;
                }
            }
        }
    }

    int hours = gameplayTime / 60 / 60;
    int minutes = (int)gameplayTime / 60 % 60;
    int seconds = (int)gameplayTime % 60;

    snprintf(buffer, sizeof(buffer), "Time: %02d:%02d:%02d", hours, minutes, seconds);

    Vector2 clockPos = {
        ui->speedSelectionButtonPannel.origin.x + 20
            + uiButtonGrid_getWidth(&ui->speedSelectionButtonPannel),
        ui->speedSelectionButtonPannel.origin.y + 5,
    };

    DrawText(buffer, clockPos.x, clockPos.y, 30, WHITE);

    // Draw cursor at the end
    Texture2D cursorTexture;

    switch (toolSelected) {
    case GARDENING_TOOL_IRRIGATOR:
        cursorTexture = cursorTexture_water;
        break;

    case GARDENING_TOOL_NUTRIENTS:
        cursorTexture = cursorTexture_feed;
        break;

    case GARDENING_TOOL_PLANTER:
        cursorTexture = cursorTexture_planter;
        break;

    case GARDENING_TOOL_PLANT_CUTTING:
        cursorTexture = cursorTexture_plant;
        break;

    case GARDENING_TOOL_TRASH_BIN:
        cursorTexture = cursorTexture_remove;
        break;

    case GARDENING_TOOL_NONE:
        cursorTexture = cursorTexture_1;
        break;

    case GARDENING_TOOL_COUNT:
        assert(false);
        break;
    }

    Vector2 mp = input->worldMousePos;
    DrawTexture(cursorTexture, mp.x - cursorTexture_1.width, mp.y, WHITE);
}
