#include "raylib.h"
#include <stdio.h>

struct Plant {
  int water;
  int nutrients;
  int health;
};

struct UIButton {
  Rectangle bounds;
  const char *label;
};

void irrigatePlant(struct Plant *p) { p->water += 30; }

void feedPlant(struct Plant *p) { p->nutrients += 20; }

void printPlant(struct Plant *p) {
  printf("My plant:\n");
  printf("- water %d\n", p->water);
  printf("- nutrients %d\n", p->nutrients);
  printf("- health %d\n", p->health);
}

bool isMouseOverButton(struct UIButton *b) {
  return CheckCollisionPointRec(GetMousePosition(), b->bounds);
}

void DrawStat(char *label, int statValue, int posX, int posY) {
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "%s: %d", label, statValue);

  DrawText(buffer, posX, posY, 20, BLACK);
}

int main(void) {
  SetTargetFPS(144);

  Vector2 screenSize = {800, 600};
  InitWindow(screenSize.x, screenSize.y, "My little plant");

  struct Plant plant = {.water = 100, .nutrients = 100, .health = 80};

  float timeSinceLasTick = 0.0f;

  const float ticksPerSecond = 1.0 / 2.0;
  const float plantTickTime = 1.0 / ticksPerSecond;

  int loops = 0;
  int maxLoops = 0;

  Vector2 uiButtonSize = {100, 40};

  struct UIButton irrigateButton = {{0, 0, uiButtonSize.x, uiButtonSize.y},
                                    "Water"};
  struct UIButton feedButton = {
      {uiButtonSize.x + 2, 0, uiButtonSize.x, uiButtonSize.y}, "Feed"};

  struct UIButton uiButtons[] = {irrigateButton, feedButton};
  // this works even if labels are different because the UIButton has a char*
  // (pointer to the string) and not the string itself
  int uiButtonsCount = sizeof(uiButtons) / sizeof(uiButtons[0]);

  // Game loop
  while ((maxLoops == 0 || loops < maxLoops) && !WindowShouldClose()) {
    float deltaTime = GetFrameTime();
    timeSinceLasTick += deltaTime;

    // Game logic
    if (timeSinceLasTick >= plantTickTime) {
      // don't set to 0 but subtract in case timer > plantTick
      timeSinceLasTick -= plantTickTime;

      plant.nutrients -= 2;
      plant.water -= 3;

      if (plant.water > 100) {
        plant.health -= (plant.water - 100) / 10;
      } else if (plant.water < 30) {
        plant.health -= (30 - plant.water) / 10;
      }

      if (plant.nutrients > 100) {
        plant.health -= (plant.nutrients - 100) / 10;
      } else if (plant.nutrients > 50) {
        plant.health += (plant.nutrients - 30) / 30;
      } else if (plant.nutrients > 30) {
        plant.health += (plant.nutrients - 30) / 20;
      } else if (plant.nutrients < 30) {
        plant.health -= (30 - plant.nutrients) / 10;
      }

      // TODO: Alerts when health is below 30
      // TODO: Game over screen when health is <= 0

      // printPlant(&plant);
    }

    // Draw
    BeginDrawing();
    ClearBackground(WHITE);

    for (int i = 0; i < uiButtonsCount; i++) {
      bool mouseOverButton = isMouseOverButton(&uiButtons[i]);
      Color buttonColor = mouseOverButton ? LIGHTGRAY : GRAY;

      DrawRectangle(uiButtons[i].bounds.x, uiButtons[i].bounds.y,
                    uiButtons[i].bounds.width, uiButtons[i].bounds.height,
                    buttonColor);

      DrawText(uiButtons[i].label, uiButtons[i].bounds.x + 30,
               uiButtons[i].bounds.y + 10, 18, WHITE);

      if (IsMouseButtonPressed(0)) {
        if (mouseOverButton) {
          switch (i) {
          // Should do something better here?
          case (0):
            irrigatePlant(&plant);
            break;

          case (1):
            feedPlant(&plant);
            break;
          }
        }
      }
    }

    DrawStat("Water", plant.water, 10, 400);
    DrawStat("Nutrients", plant.nutrients, 10, 430);
    DrawStat("Health", plant.health, 10, 460);

    EndDrawing();

    // End game logic
    loops++;
  }

  return 0;
}
