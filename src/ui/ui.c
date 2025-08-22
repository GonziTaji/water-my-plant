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

    if (garden->tileSelected != -1 && garden->tiles[garden->tileSelected].hasPlanter &&
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

        const char *title = "Plant stats";

        int statsCount = sizeof(stats) / sizeof(stats[0]);
        char buffer[64];

        int titleFontSize = 36;
        int statFontSize = 32;

        float widestText = MeasureTextEx(ui->font, title, titleFontSize, 0).x;

        for (int i = 0; i < statsCount; i++) {
            // TODO: MAX_STAT_VALUE?
            snprintf(buffer, sizeof(buffer), "%s: %d", stats[i].label, 1000);
            float statTextWidth = widestText = MeasureTextEx(ui->font, buffer, statFontSize, 0).x;

            if (statTextWidth > widestText) {
                widestText = statTextWidth;
            }
        }

        Vector2 textPos = {screenSize->x - widestText - 20, 20};

        DrawTextEx(ui->font, title, textPos, titleFontSize, 0, BLACK);
        textPos.y += 30;

        for (int i = 0; i < statsCount; i++) {
            snprintf(buffer, sizeof(buffer), "%s: %d", stats[i].label, stats[i].value);
            DrawTextEx(ui->font, buffer, textPos, statFontSize, 0, BLACK);
            textPos.y += 30;
        }
    }
}

void ui_unloadResources(UI *ui) {
    UnloadFont(ui->font);
}
