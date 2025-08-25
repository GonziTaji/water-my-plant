#include "ui.h"
#include "../entity/planter.h"
#include "button.h"
#include "commands.h"
#include "input_manager.h"
#include "raylib.h"
#include <assert.h>
#include <stdio.h>

void ui_init(UI *ui) {
    const char *fontUrl = "resources/fonts/friendlyscribbles/friendlyscribbles.ttf";

    ui->font = LoadFontEx(fontUrl, 64, NULL, 0);

    printf("Font base size: %d\n", ui->font.baseSize);

    SetTextureFilter(ui->font.texture, TEXTURE_FILTER_BILINEAR);

    UIButton uiButtons[] = {
        (UIButton){.label = "[W]ater", .command = command_irrigate},
        (UIButton){.label = "[F]eed", .command = command_feed},
        (UIButton){.label = "Add planter [M]", .command = command_addPlanter},
        (UIButton){.label = "Remove planter [C]", .command = command_removePlanter},
        (UIButton){.label = "[A]dd plant", .command = command_addPlant},
        (UIButton){.label = "[R]emove plant", .command = command_removePlant},
        (UIButton){.label = "[N]ext tile", .command = command_focusNextTile},
        (UIButton){.label = "[P]rev tile", .command = command_focusPreviousTile},
    };

    ui->buttonsCount = sizeof(uiButtons) / sizeof(uiButtons[0]);

    assert(ui->buttonsCount <= BUTTON_COLS * BUTTON_ROWS);

    for (int i = 0; i < ui->buttonsCount; i++) {
        int col = i == 0 ? 0 : i % BUTTON_COLS;
        int row = i == 0 ? 0 : i / BUTTON_COLS;
        float x =
            BUTTON_GRID_ORIGIN_X + BUTTON_GRID_PADDING_X + col * (BUTTON_WIDTH + BUTTON_SPACING_X);
        float y =
            BUTTON_GRID_ORIGIN_Y + BUTTON_GRID_PADDING_Y + row * (BUTTON_HEIGHT + BUTTON_SPACING_Y);

        uiButtons[i].bounds = (Rectangle){x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
        uiButtons[i].isMouseOver = false;

        ui->buttons[i] = uiButtons[i];
    }
}

void ui_processInput(UI *ui, InputManager *input, Garden *garden) {
    for (int i = 0; i < ui->buttonsCount; i++) {
        ui->buttons[i].isMouseOver = button_isMouseOver(&ui->buttons[i], input);
    }

    if (input->mouseButtonPressed[0]) {
        for (int i = 0; i < ui->buttonsCount; i++) {
            if (ui->buttons[i].isMouseOver) {
                input->mouseButtonPressed[0] = false;
                ui->buttons[i].command(garden);
                return;
            }
        }
    }
}

void ui_draw(UI *ui, const Vector2 *screenSize, const Garden *garden) {
    int buttonGridHeight = (BUTTON_ROWS * BUTTON_HEIGHT) + ((BUTTON_ROWS - 1) * BUTTON_SPACING_Y) +
                           (BUTTON_GRID_PADDING_Y * 2);
    int buttonGridWidth = (BUTTON_COLS * BUTTON_WIDTH) + ((BUTTON_COLS - 1) * BUTTON_SPACING_X) +
                          (BUTTON_GRID_PADDING_X * 2);

    DrawRectangle(
        BUTTON_GRID_ORIGIN_X, BUTTON_GRID_ORIGIN_Y, buttonGridWidth, buttonGridHeight, BLACK);

    for (int i = 0; i < ui->buttonsCount; i++) {
        Color buttonColor = ui->buttons[i].isMouseOver ? LIGHTGRAY : GRAY;

        DrawRectangle(ui->buttons[i].bounds.x,
            ui->buttons[i].bounds.y,
            ui->buttons[i].bounds.width,
            ui->buttons[i].bounds.height,
            buttonColor);

        int fontSize = ui->font.baseSize * 0.5f;
        Vector2 textSize = MeasureTextEx(ui->font, ui->buttons[i].label, fontSize, 0);
        Vector2 textPos = {
            ui->buttons[i].bounds.x + (ui->buttons[i].bounds.width - textSize.x) / 2,
            ui->buttons[i].bounds.y + (ui->buttons[i].bounds.height - textSize.y) / 2,
        };

        DrawTextEx(ui->font, ui->buttons[i].label, textPos, fontSize, 0, WHITE);
    }

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

        DrawTextEx(ui->font, buffer, textPos, fontSize, fontSpacing, BLACK);

        textPos.y += fontSize;

        DrawTextEx(ui->font, "Plant:", textPos, fontSize, fontSpacing, BLACK);

        textPos.y += fontSize;

        if (garden->tiles[garden->tileSelected].hasPlanter &&
            garden->tiles[garden->tileSelected].planter.hasPlant) {
            const Planter *selectedPlanter = &garden->tiles[garden->tileSelected].planter;

            struct {
                const char *label;
                int value;
            } stats[] = {
                {"Water", selectedPlanter->plant.water},
                {"Nutrients", selectedPlanter->plant.nutrients},
                {"Health", selectedPlanter->plant.health},
            };

            int statsCount = 3;

            char buffer[64];

            for (int i = 0; i < statsCount; i++) {
                snprintf(buffer, sizeof(buffer), "%s: %d", stats[i].label, stats[i].value);
                DrawTextEx(ui->font, buffer, textPos, fontSize, 0, BLACK);
                textPos.y += fontSize;
            }
        } else {
            DrawTextEx(ui->font, "None", textPos, fontSize, 0, BLACK);
        }
    }
}

void ui_unloadResources(UI *ui) {
    UnloadFont(ui->font);
}
