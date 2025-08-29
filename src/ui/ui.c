#include "ui.h"
#include "../core/asset_manager.h"
#include "../entity/planter.h"
#include "button.h"
#include "button_pannel.h"
#include "input_manager.h"
#include "raylib.h"
#include <assert.h>
#include <stdio.h>

void ui_init(UI *ui, Vector2 *screenSize) {
    HideCursor();
    ui->showPlantSelection = false;

    UIButton toolButtons[PANNEL_MAX_BUTTONS];
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

        toolButtons[toolButtonsCount] = button_create(BUTTON_TYPE_TEXT_LABEL,
            bcontent,
            (Command){COMMAND_TOOL_SELECTED, {.tool = toolIndex}});

        toolButtonsCount++;
    }

    // main button pannel:
    buttonPannel_init(&ui->toolSelectionButtonPannel,
        1,
        toolButtonsCount,
        (Vector2i){160, 40},
        (Vector2i){2, 2},
        (Vector2i){4, 4},
        (Vector2i){20, 20},
        toolButtons);

    UIButton plantSelectionButtons[PANNEL_MAX_BUTTONS];

    for (int i = 0; i < PLANT_TYPE_COUNT; i++) {
        plantSelectionButtons[i] = button_create(BUTTON_TYPE_SPRITE,
            (ButtonContent){.icon = {plantAtlas, plant_getSpriteSourceRect(i, 100)}},
            (Command){COMMAND_ADD_PLANT, {.plantType = i}});
    }

    buttonPannel_init(&ui->plantSelectionButtonPannel,
        PLANT_TYPE_COUNT,
        1,
        (Vector2i){40, 40},
        (Vector2i){2, 2},
        (Vector2i){2, 2},
        (Vector2i){0, 0},
        plantSelectionButtons);
}

Command ui_processInput(UI *ui, InputManager *input) {
    if (ui->showPlantSelection) {
        Command cmd = buttonPannel_processInput(&ui->plantSelectionButtonPannel, input);

        if (cmd.type != COMMAND_NONE) {
            return cmd;
        }

        if (input->mouseButtonPressed == MOUSE_BUTTON_LEFT) {
            ui->showPlantSelection = false;
        }
    }

    Command cmd = buttonPannel_processInput(&ui->toolSelectionButtonPannel, input);

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

    int fontSize = uiFont.baseSize * 0.4f;
    buttonPannel_draw(&ui->toolSelectionButtonPannel, fontSize);

    if (ui->showPlantSelection) {
        buttonPannel_draw(&ui->plantSelectionButtonPannel, fontSize);
    }

    // draw selected tile outline
    if (garden->tileSelected != -1) {
        bool hasPlant = garden->tiles[garden->tileSelected].hasPlanter &&
                        garden->tiles[garden->tileSelected].planter.hasPlant;

        int fontSize = 30;
        int tileInfoRecPadding = 20;
        int tileInfoRecWidth = 300;

        // Height = 2 lines of text and padding top and bottom
        Rectangle tileInfoPos = {screenSize->x - tileInfoRecWidth,
            0,
            tileInfoRecWidth,
            tileInfoRecPadding * 2 + fontSize * 2};

        if (hasPlant) {
            // Space for 3 lines of text for plant stats
            tileInfoPos.height += fontSize * 3;
        } else {
            // Space for 1 line of text for "None" (no plant)
            tileInfoPos.height += fontSize;
        }

        DrawRectangle(tileInfoPos.x, tileInfoPos.y, tileInfoPos.width, tileInfoPos.height, GRAY);

        char buffer[64];
        int fontSpacing = 0;

        snprintf(buffer,
            sizeof(buffer),
            "Light level: %d",
            garden->tiles[garden->tileSelected].lightLevel);

        Vector2 textPos = {tileInfoPos.x + tileInfoRecPadding, tileInfoPos.y + tileInfoRecPadding};

        DrawTextEx(uiFont, buffer, textPos, fontSize, fontSpacing, BLACK);

        textPos.y += fontSize;

        DrawTextEx(uiFont, "Plant:", textPos, fontSize, fontSpacing, BLACK);

        textPos.y += fontSize;

        if (garden->tiles[garden->tileSelected].hasPlanter &&
            garden->tiles[garden->tileSelected].planter.hasPlant) {
            const Planter *selectedPlanter = &garden->tiles[garden->tileSelected].planter;

            struct {
                const char *label;
                int value;
            } stats[] = {
                {"Type", selectedPlanter->plant.type},
                {"Water", selectedPlanter->plant.water},
                {"Nutrients", selectedPlanter->plant.nutrients},
                {"Health", selectedPlanter->plant.health},
            };

            int statsCount = 3;

            char buffer[64];

            for (int i = 0; i < statsCount; i++) {
                snprintf(buffer, sizeof(buffer), "%s: %d", stats[i].label, stats[i].value);
                DrawTextEx(uiFont, buffer, textPos, fontSize, 0, BLACK);
                textPos.y += fontSize;
            }
        } else {
            DrawTextEx(uiFont, "None", textPos, fontSize, 0, BLACK);
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
