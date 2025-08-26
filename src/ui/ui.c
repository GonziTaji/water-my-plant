#include "ui.h"
#include "../core/asset_manager.h"
#include "../entity/planter.h"
#include "button.h"
#include "button_pannel.h"
#include "commands.h"
#include "input_manager.h"
#include "raylib.h"
#include <assert.h>
#include <stdio.h>

static bool showPlantSelection = false;

void tmpCommant_showPlantSelection(Garden *g) {
    showPlantSelection = true;
}

void tmpCommant_hidePlantSelection(Garden *g) {
    showPlantSelection = false;
}

void ui_init(UI *ui, Vector2 *screenSize) {
    // main button pannel:
    buttonPannel_init(&ui->mainButtonPannel,
        1,
        8,
        (Vector2i){200, 80},
        (Vector2i){8, 8},
        (Vector2i){8, 8},
        (Vector2i){20, 20},
        (UIButton[PANNEL_MAX_BUTTONS]){
            (UIButton){.label = "[W]ater", .command = command_irrigate},
            (UIButton){.label = "[F]eed", .command = command_feed},
            (UIButton){.label = "Add planter [M]", .command = command_addPlanter},
            (UIButton){.label = "Remove planter [C]", .command = command_removePlanter},
            // (UIButton){.label = "[A]dd plant", .command = command_addPlant},
            (UIButton){.label = "[A]dd plant", .command = tmpCommant_showPlantSelection},
            (UIButton){.label = "[R]emove plant", .command = command_removePlant},
            (UIButton){.label = "[N]ext tile", .command = command_focusNextTile},
            (UIButton){.label = "[P]rev tile", .command = command_focusPreviousTile},
        });

    UIButton plantSelectionButtons[PANNEL_MAX_BUTTONS];

    for (int i = 0; i < sizeof(enum PLANT_TYPE); i++) {
        char *plantType = "";
        switch (i) {
        case 0:
            plantType = "A";
            break;
        case 1:
            plantType = "B";
            break;
        case 2:
            plantType = "C";
        }

        plantSelectionButtons[i] = (UIButton){
            plantType,
            tmpCommant_hidePlantSelection,
        };
    }

    buttonPannel_init(&ui->plantSelectionButtonPannel,
        3,
        1,
        (Vector2i){100, 100},
        (Vector2i){8, 8},
        (Vector2i){8, 8},
        (Vector2i){100, screenSize->y - 200},
        plantSelectionButtons);
}

void ui_processInput(UI *ui, InputManager *input, Garden *garden) {
    buttonPannel_processInput(&ui->mainButtonPannel, input, garden);

    if (showPlantSelection) {
        buttonPannel_processInput(&ui->plantSelectionButtonPannel, input, garden);

        if (input->mouseButtonPressed[MOUSE_BUTTON_LEFT]) {
            showPlantSelection = false;
            input->mouseButtonPressed[MOUSE_BUTTON_LEFT] = false;
        }
    }
}

void ui_draw(UI *ui, const Vector2 *screenSize, const Garden *garden) {
    buttonPannel_draw(&ui->mainButtonPannel);

    if (showPlantSelection) {
        buttonPannel_draw(&ui->plantSelectionButtonPannel);
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
}
