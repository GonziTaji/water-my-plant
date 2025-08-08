#include "plant.h"
#include "raylib.h"
#include <stdio.h>

static const float PLANT_TICKS_PER_SECOND = 0.5f;
static const float PLANT_TICK_TIME = 1.0f / PLANT_TICKS_PER_SECOND;

void plant_init(Plant *p) {
    p->water = 100;
    p->nutrients = 100;
    p->health = 80;
    p->timeSinceLastTick = 0;
}

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
