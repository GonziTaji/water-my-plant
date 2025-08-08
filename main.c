#include "raylib.h"
#include <stdio.h>

static const float PLANT_TICKS_PER_SECOND = 0.5f;
static const float PLANT_TICK_TIME = 1.0f / PLANT_TICKS_PER_SECOND;

typedef struct {
    int water;
    int nutrients;
    int health;
    float timeSinceLastTick;
} Plant;

typedef enum {
    BUTTON_ACTION_IRRIGATE,
    BUTTON_ACTION_FEED,
} ButtonActionType;

typedef struct {
    Rectangle bounds;
    const char *label;
    bool isMouseOver;
    int mouseButtonClicked;
    ButtonActionType buttonAction;
} UIButton;

void plant_irrigate(Plant *p) {
    p->water += 30;
}

void plant_feed(Plant *p) {
    p->nutrients += 20;
}

void plant_printStats(Plant *p) {
    printf("My plant:\n");
    printf("- water %d\n", p->water);
    printf("- nutrients %d\n", p->nutrients);
    printf("- health %d\n", p->health);
}

void plant_update(Plant *plant, float deltaTime) {
    plant->timeSinceLastTick += deltaTime;

    if (plant->timeSinceLastTick >= PLANT_TICK_TIME) {
        // don't set to 0 but subtract in case timer > PLANT_TICK_TIME
        plant->timeSinceLastTick -= PLANT_TICK_TIME;

        plant->nutrients -= 2;
        plant->water -= 3;

        if (plant->water > 100) {
            plant->health -= (plant->water - 100) / 10;
        } else if (plant->water < 30) {
            plant->health -= (30 - plant->water) / 10;
        }

        if (plant->nutrients > 100) {
            plant->health -= (plant->nutrients - 100) / 10;
        } else if (plant->nutrients > 50) {
            plant->health += (plant->nutrients - 30) / 30;
        } else if (plant->nutrients > 30) {
            plant->health += (plant->nutrients - 30) / 20;
        } else if (plant->nutrients < 30) {
            plant->health -= (30 - plant->nutrients) / 10;
        }
    }
}

void plant_draw(Plant *plant) {
    struct {
        const char *label;
        int value;
    } stats[] = {
        {"Water", plant->water},
        {"Nutrients", plant->nutrients},
        {"Health", plant->health},
    };

    int statsCount = sizeof(stats) / sizeof(stats[0]);
    char buffer[64];

    int y = 400;
    for (int i = 0; i < statsCount; i++) {
        snprintf(buffer, sizeof(buffer), "%s: %d", stats[i].label, stats[i].value);
        DrawText(buffer, 10, y, 20, BLACK);
        y += 30;
    }
}

bool button_isMouseOver(UIButton *b) {
    return CheckCollisionPointRec(GetMousePosition(), b->bounds);
}

void ui_processInput(UIButton *uiButtonList, int uiButtonsCount, Plant *plant) {
    for (int i = 0; i < uiButtonsCount; i++) {
        uiButtonList[i].isMouseOver = button_isMouseOver(&uiButtonList[i]);

        if (IsMouseButtonPressed(0) && uiButtonList[i].isMouseOver) {
            switch (uiButtonList[i].buttonAction) {
            // Should do something better here?
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

void ui_draw(UIButton *uiButtonList, int uiButtonsCount) {
    for (int i = 0; i < uiButtonsCount; i++) {
        Color buttonColor = uiButtonList[i].isMouseOver ? LIGHTGRAY : GRAY;

        DrawRectangle(uiButtonList[i].bounds.x,
            uiButtonList[i].bounds.y,
            uiButtonList[i].bounds.width,
            uiButtonList[i].bounds.height,
            buttonColor);

        DrawText(uiButtonList[i].label,
            uiButtonList[i].bounds.x + 30,
            uiButtonList[i].bounds.y + 10,
            18,
            WHITE);
    }
}

int main(void) {
    SetTargetFPS(144);

    Vector2 screenSize = {800, 600};
    InitWindow(screenSize.x, screenSize.y, "My little plant");

    Plant plant = {.water = 100, .nutrients = 100, .health = 80, 0};

    int loops = 0;
    int maxLoops = 0;

    Vector2 uiButtonSize = {100, 40};

    UIButton irrigateButton = {
        {0, 0, uiButtonSize.x, uiButtonSize.y}, "Water", false, BUTTON_ACTION_IRRIGATE};
    UIButton feedButton = {
        {uiButtonSize.x + 2, 0, uiButtonSize.x, uiButtonSize.y}, "Feed", false, BUTTON_ACTION_FEED};

    UIButton uiButtons[] = {irrigateButton, feedButton};
    // this works even if labels are different because the UIButton has a char*
    // (pointer to the string) and not the string itself
    int uiButtonsCount = sizeof(uiButtons) / sizeof(uiButtons[0]);

    /* **
     * Game loop
     */
    while ((maxLoops == 0 || loops < maxLoops) && !WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // User input
        ui_processInput(uiButtons, uiButtonsCount, &plant);

        // Update
        plant_update(&plant, deltaTime);

        // Draw
        BeginDrawing();
        ClearBackground(WHITE);

        ui_draw(uiButtons, uiButtonsCount);
        plant_draw(&plant);

        EndDrawing();

        // End game logic
        loops++;
    }

    return 0;
}
