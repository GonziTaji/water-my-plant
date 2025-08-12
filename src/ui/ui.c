#include "ui.h"
#include "../entity/planter.h"
#include "button.h"
#include "commands.h"
#include "raylib.h"
#include <assert.h>

void ui_init(UI *ui) {
    UIButton uiButtons[] = {
        (UIButton){.label = "[W]ater", .command = command_irrigate},
        (UIButton){.label = "[F]eed", .command = command_feed},
        (UIButton){.label = "[N]ext planter", .command = command_focusNextPlanter},
        (UIButton){.label = "[P]previous planter", .command = command_focusPreviousPlanter},
        (UIButton){.label = "[A]dd plant", .command = command_addPlant},
        (UIButton){.label = "[R]emove plant", .command = command_removePlant},
    };

    ui->buttonsCount = sizeof(uiButtons) / sizeof(uiButtons[0]);

    assert(ui->buttonsCount <= BUTTON_COLS * BUTTON_ROWS);

    for (int i = 0; i < ui->buttonsCount; i++) {
        int col = i == 0 ? 0 : i % BUTTON_COLS;
        int row = i == 0 ? 0 : i / BUTTON_COLS;
        float x = BUTTON_GRID_ORIGIN_X + col * (BUTTON_WIDTH + BUTTON_SPACING_X);
        float y = BUTTON_GRID_ORIGIN_Y + row * (BUTTON_HEIGHT + BUTTON_SPACING_Y);

        uiButtons[i].bounds = (Rectangle){x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
        uiButtons[i].isMouseOver = false;

        ui->buttons[i] = uiButtons[i];
    }
}

void ui_processInput(UI *ui, Garden *garden) {
    for (int i = 0; i < ui->buttonsCount; i++) {
        ui->buttons[i].isMouseOver = button_isMouseOver(&ui->buttons[i]);
    }

    if (IsMouseButtonPressed(0)) {
        int clickedPlanterIndex = garden_getPlanterIndexFromPoint(GetMousePosition());

        if (clickedPlanterIndex != -1) {
            garden->selectedPlanter = clickedPlanterIndex;
        } else {
            for (int i = 0; i < ui->buttonsCount; i++) {
                if (ui->buttons[i].isMouseOver) {
                    ui->buttons[i].command(garden);
                }
            }
        }
    }
}

void ui_draw(UI *ui, Planter *selectedPlanter) {
    for (int i = 0; i < ui->buttonsCount; i++) {
        Color buttonColor = ui->buttons[i].isMouseOver ? LIGHTGRAY : GRAY;

        DrawRectangle(ui->buttons[i].bounds.x,
            ui->buttons[i].bounds.y,
            ui->buttons[i].bounds.width,
            ui->buttons[i].bounds.height,
            buttonColor);

        DrawText(ui->buttons[i].label,
            ui->buttons[i].bounds.x + 10,
            ui->buttons[i].bounds.y + 10,
            12,
            WHITE);
    }

    if (selectedPlanter->hasPlant) {
        plant_drawStats(&selectedPlanter->plant, (Vector2){200, 200});
    }
}
