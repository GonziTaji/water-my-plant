#include "ui.h"
#include "../entity/planter.h"
#include "button.h"
#include "commands.h"
#include "raylib.h"
#include <assert.h>
#include <stdio.h>

void ui_init(UI *ui) {
    Vector2 uiButtonSize = {100, 40};

    UIButton irrigateButton = {
        {0, 0, uiButtonSize.x, uiButtonSize.y}, "Water", false, 0, command_irrigate};
    UIButton feedButton = {
        {uiButtonSize.x + 2, 0, uiButtonSize.x, uiButtonSize.y}, "Feed", false, 0, command_feed};

    UIButton uiButtons[] = {irrigateButton, feedButton};
    ui->buttonsCount = sizeof(uiButtons) / sizeof(uiButtons[0]);

    assert(ui->buttonsCount <= UI_MAX_BUTTONS_COUNT);

    for (int i = 0; i < ui->buttonsCount; i++) {
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
                    Planter *selectedPlanter = &garden->planters[garden->selectedPlanter];

                    if (selectedPlanter->hasPlant) {
                        ui->buttons[i].command(garden);
                    }
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
            ui->buttons[i].bounds.x + 30,
            ui->buttons[i].bounds.y + 10,
            16,
            WHITE);
    }

    if (selectedPlanter->hasPlant) {
        plant_drawStats(&selectedPlanter->plant, (Vector2){200, 200});
    }

    char buffer[64];

    int x = 10;
    int y = 200;

    Vector2 mp = GetMousePosition();
    snprintf(buffer, sizeof(buffer), "x: %f, y: %f", mp.x, mp.y);
    DrawText(buffer, x, y, 16, BLACK);
}
