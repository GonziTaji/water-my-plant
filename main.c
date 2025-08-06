#include "raylib.h"
#include <stdio.h>

struct Plant {
  int water;
  int nutrients;
  int health;
};

void irrigate(struct Plant *p) { p->water += 10; };

void addNutrients(struct Plant *p) { p->nutrients += 10; };

void printPlant(struct Plant *p) {
  printf("My plant:\n");
  printf("- water %d\n", p->water);
  printf("- nutrients %d\n", p->nutrients);
  printf("- health %d\n", p->health);
}

int main(void) {
  SetTargetFPS(144);
  InitWindow(800, 600, "plantita");

  struct Plant plant = {.water = 100, .nutrients = 100, .health = 100};

  float timer = 0.0f;

  const float plantTick = 1.0f; // 1 second
  int loops = 0;
  int maxLoops = 0;

  // Game loop
  while (maxLoops == 0 || loops < maxLoops || WindowShouldClose()) {
    float deltaTime = GetFrameTime();
    timer += deltaTime;

    // printf("Timer: %f - Delta: %f\n", timer, deltaTime);

    // Game logic
    if (timer >= plantTick) {
      // don't set to 0 but subtract in case timer > plantTick
      timer -= plantTick;

      plant.nutrients -= 1;
      plant.water -= 5;
      printPlant(&plant);
    }

    // Draw
    BeginDrawing();
    EndDrawing();

    // End game logic
    loops++;
  }

  return 0;
}
