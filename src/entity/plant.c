#include "plant.h"
#include "raylib.h"
#include <assert.h>
#include <stdio.h>

#define PLANT_STATE_COUNT 6

static const float PLANT_TICKS_PER_SECOND = 2.0f;
static const float PLANT_TICK_TIME = 1.0f / PLANT_TICKS_PER_SECOND;

static Texture2D plantTextures[PLANT_STATE_COUNT];

void plant_init(Plant *p) {
    p->water = 100;
    p->nutrients = 100;
    p->health = 80;
    p->timeSinceLastTick = 0;
    p->healthStatusIndex = 0;
}

void plant_loadTextures() {
    plantTextures[0] = LoadTexture("assets/plant_1.png");
    plantTextures[1] = LoadTexture("assets/plant_2.png");
    plantTextures[2] = LoadTexture("assets/plant_3.png");
    plantTextures[3] = LoadTexture("assets/plant_4.png");
    plantTextures[4] = LoadTexture("assets/plant_5.png");
    plantTextures[5] = LoadTexture("assets/plant_6.png");
}

void plant_unloadTextures() {
    for (int i = 1; i < PLANT_STATE_COUNT; i++) {
        if (plantTextures[i].id != 0) {
            UnloadTexture(plantTextures[i]);
        }
    }
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

    if (plant->health > 70) {
        plant->healthStatusIndex = 0;
    } else if (plant->health > 50) {
        plant->healthStatusIndex = 1;
    } else if (plant->health > 40) {
        plant->healthStatusIndex = 2;
    } else if (plant->health > 20) {
        plant->healthStatusIndex = 3;
    } else if (plant->health > 10) {
        plant->healthStatusIndex = 4;
    } else {
        plant->healthStatusIndex = 5;
    }
}

void plant_draw(Plant *plant, Vector2 origin) {
    float textureScale = 2.0;

    Texture2D t = plantTextures[plant->healthStatusIndex];

    assert(t.id != 0);

    DrawTextureEx(t, origin, 0, textureScale, WHITE);
}

void plant_drawStats(Plant *plant, Vector2 origin) {
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

    int x = origin.x;
    int y = origin.y;

    for (int i = 0; i < statsCount; i++) {
        snprintf(buffer, sizeof(buffer), "%s: %d", stats[i].label, stats[i].value);
        DrawText(buffer, x, y, 16, BLACK);
        y += 30;
    }
}
