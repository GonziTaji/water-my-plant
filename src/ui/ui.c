#include "ui.h"
#include "../entity/plant.h"
#include "button.h"
#include "raylib.h"
#include <assert.h>
#include <stdio.h>

void ui_init(UI *ui) {
    Vector2 uiButtonSize = {100, 40};

    UIButton irrigateButton = {
        {0, 0, uiButtonSize.x, uiButtonSize.y}, "Water", false, 0, BUTTON_ACTION_IRRIGATE};
    UIButton feedButton = {
        {uiButtonSize.x + 2, 0, uiButtonSize.x, uiButtonSize.y}, "Feed", false, 0, BUTTON_ACTION_FEED};

    UIButton uiButtons[] = {irrigateButton, feedButton};
    ui->buttonsCount = sizeof(uiButtons) / sizeof(uiButtons[0]);

    assert(ui->buttonsCount <= UI_MAX_BUTTON_COUNT);

    for (int i = 0; i < ui->buttonsCount; i++) {
        ui->buttons[i] = uiButtons[i];
    }
}

// maneja input y actualiza elementos de la UI
void ui_update(UI *ui, Plant *plant) {
    for (int i = 0; i < ui->buttonsCount; i++) {
        ui->buttons[i].isMouseOver = button_isMouseOver(&ui->buttons[i]);

        if (IsMouseButtonPressed(0) && ui->buttons[i].isMouseOver) {
            switch (ui->buttons[i].buttonAction) {
            case (BUTTON_ACTION_IRRIGATE):
                plant_irrigate(plant);
                break;

            case (BUTTON_ACTION_FEED):
                plant_feed(plant);
                break;
            }
        }
    }
}

void ui_draw(UI *ui) {
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
            18,
            WHITE);
    }
}
