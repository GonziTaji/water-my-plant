#include "ui.h"
#include "../core/asset_manager.h"
#include "../entity/planter.h"
#include "button.h"
#include "input_manager.h"
#include "raylib.h"
#include "ui_button_grid.h"
#include "ui_text_box.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

int getButtonGridFullWidth(UIButtonGrid *grid) {
    return ((grid->padding.x * 2) + ((grid->cols - 1) * grid->buttonSpacing.x) +
            (grid->cols * grid->buttonDimensions.x));
}

int getButtonGridFullHeight(UIButtonGrid *grid) {
    return ((grid->padding.y * 2) + ((grid->rows - 1) * grid->buttonSpacing.y) +
            (grid->rows * grid->buttonDimensions.y));
}

void ui_init(UI *ui, Vector2 *screenSize, GameplaySpeed gameplaySpeed) {
    HideCursor();

    UIButton toolButtons[UI_BUTTON_PANNEL_MAX_BUTTONS];
    ButtonContent bcontent = (ButtonContent){.label = ""};
    int toolButtonsCount = 0;

    for (int toolIndex = 0; toolIndex < GARDENING_TOOL_COUNT; toolIndex++) {
        switch (toolIndex) {
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

        case GARDENING_TOOL_NONE:
        case GARDENING_TOOL_COUNT:
            continue;
        }

        toolButtons[toolButtonsCount] = (UIButton){
            .type = BUTTON_TYPE_TEXT_LABEL,
            .content = bcontent,
            .command = (Command){COMMAND_TOOL_SELECTED, {.tool = toolIndex}},
        };

        toolButtonsCount++;
    }

    uiButtonGrid_init(&ui->toolSelectionButtonPannel,
        toolButtonsCount,
        1,
        (Vector2i){160, 80},
        (Vector2i){4, 4},
        (Vector2i){4, 4},
        (Vector2i){0, 0},
        toolButtons);

    UIButtonGrid *toolGrid = &ui->toolSelectionButtonPannel;

    float toolGridWidth = getButtonGridFullWidth(toolGrid);
    float toolGridHeight = getButtonGridFullHeight(toolGrid);

    Vector2 toolGridPos = {
        (screenSize->x - toolGridWidth) / 2,
        screenSize->y - (toolGridHeight + 20),
    };

    uiButtonGrid_tranform(toolGrid, toolGridPos);

    UIButton plantSelectionButtons[UI_BUTTON_PANNEL_MAX_BUTTONS];

    for (int i = 0; i < PLANT_TYPE_COUNT; i++) {
        plantSelectionButtons[i] = (UIButton){
            .type = BUTTON_TYPE_SPRITE,
            .content = (ButtonContent){.icon = {plantAtlas, plant_getSpriteSourceRect(i, 100)}},
            .command = (Command){COMMAND_PLANT_TYPE_SELECTED, {.plantType = i}},
            .status = i == 0 ? BUTTON_STATUS_ACTIVE : BUTTON_STATUS_NORMAL,
        };
    }

    uiButtonGrid_init(&ui->plantSelectionButtonPannel,
        PLANT_TYPE_COUNT,
        1,
        (Vector2i){80, 80},
        (Vector2i){4, 4},
        (Vector2i){4, 4},
        (Vector2i){0, 0},
        plantSelectionButtons);

    UIButtonGrid *plantGrid = &ui->plantSelectionButtonPannel;

    Vector2 plantGridPos = (Vector2){
        (screenSize->x - getButtonGridFullWidth(plantGrid)) / 2,
        toolGrid->origin.y - (getButtonGridFullHeight(plantGrid) + 20),
    };

    uiButtonGrid_tranform(plantGrid, plantGridPos);

    UIButton speedSelectionButtons[UI_BUTTON_PANNEL_MAX_BUTTONS];

    for (int i = 0; i < GAMEPLAY_SPEED_COUNT; i++) {
        speedSelectionButtons[i] = (UIButton){
            .type = BUTTON_TYPE_TEXT_LABEL,
            .content = {.label = ">"},
            .command = (Command){COMMAND_GAMEPLAY_CHANGE_SPEED, {.gameplaySpeed = i}},
            .status = gameplaySpeed == i ? BUTTON_STATUS_ACTIVE : BUTTON_STATUS_NORMAL,
        };

        uiButtonGrid_init(&ui->speedSelectionButtonPannel,
            GAMEPLAY_SPEED_COUNT,
            1,
            (Vector2i){80, 30},
            (Vector2i){4, 4},
            (Vector2i){4, 4},
            (Vector2i){100, 10},
            speedSelectionButtons);
    }
}

Command ui_processInput(UI *ui, InputManager *input, enum GardeningTool tool) {
    if (tool == GARDENING_TOOL_PLANT_CUTTING) {
        Command cmd = uiButtonGrid_processInput(&ui->plantSelectionButtonPannel, input);

        if (cmd.type != COMMAND_NONE) {
            return cmd;
        }
    }

    Command cmd = uiButtonGrid_processInput(&ui->toolSelectionButtonPannel, input);

    if (cmd.type != COMMAND_NONE) {
        return cmd;
    }

    cmd = uiButtonGrid_processInput(&ui->speedSelectionButtonPannel, input);

    if (cmd.type != COMMAND_NONE) {
        return cmd;
    }

    return (Command){COMMAND_NONE};
}

void ui_draw(UI *ui,
    const InputManager *input,
    const Vector2 *screenSize,
    const Garden *garden,
    enum GardeningTool toolSelected) {

    int bpFontSize = uiFont.baseSize;
    uiButtonGrid_draw(&ui->toolSelectionButtonPannel, bpFontSize);

    uiButtonGrid_draw(&ui->speedSelectionButtonPannel, bpFontSize);

    if (toolSelected == GARDENING_TOOL_PLANT_CUTTING) {
        uiButtonGrid_draw(&ui->plantSelectionButtonPannel, bpFontSize);
    }

    const GardenTile *tile;
    bool tileExist = true;
    if (garden->tileHovered != -1) {
        tile = &garden->tiles[garden->tileHovered];
    } else if (garden->tileSelected != -1) {
        tile = &garden->tiles[garden->tileSelected];
    } else {
        tileExist = false;
    }

    int fontSize = uiFont.baseSize;
    int tileInfoRecWidth = 250;

    if (tileExist) {
        Vector2 offset = {-20, 20};
        Rectangle tbBounds = {
            offset.x + screenSize->x - tileInfoRecWidth,
            offset.y,
            tileInfoRecWidth,
            screenSize->y - (offset.y * 2),
        };
        UITextBox tb;
        uiTextBox_init(&tb, fontSize, tbBounds, (Vector2){20, 20});

        DrawRectangleRec(tbBounds, GRAY);

        char buffer[64];

        snprintf(buffer, sizeof(buffer), "Light level: %d", tile->lightLevel);

        uiTextBox_drawTextLine(&tb, buffer, BLACK);
        uiTextBox_drawTextLine(&tb, "", BLACK); // spacing

        if (tile->hasPlanter && tile->planter.hasPlant) {
            const Plant *plant = &tile->planter.plant;

            uiTextBox_drawTextLine(&tb, "Plant info:", BLACK);
            tb.cursorPosition.y += 5; // spacing

            char buffer[64];

            struct {
                const char *label;
                const char *value;
            } infoArr[] = {
                {"Scientific name", plant->scientificName},
                {"Name", plant->name},
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

                DrawRectangleV(tb.cursorPosition, (Vector2){internalWidth, fontSize}, LIGHTGRAY);
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
