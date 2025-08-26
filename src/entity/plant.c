#include "plant.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "raylib.h"
#include <assert.h>
#include <stdio.h>

#define PLANT_STATE_COUNT 6

static const float PLANT_TICKS_PER_SECOND = 0.5f;
static const float PLANT_TICK_TIME = 1.0f / PLANT_TICKS_PER_SECOND;

void plant_init(Plant *p, enum PLANT_TYPE type) {
    p->type = type;
    p->water = 100;
    p->nutrients = 100;
    p->health = 80;
    p->timeSinceLastTick = 0;
    p->healthStatusIndex = 0;
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

        if (plant->nutrients < 0) {
            plant->nutrients = 0;
        }

        if (plant->water < 0) {
            plant->water = 0;
        }

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

        if (plant->health < 0) {
            plant->health = 0;
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

/// The plant is drawn with the center of its base at the origin
void plant_draw(Plant *plant, Vector2 origin) {
    Rectangle source = {
        plant->healthStatusIndex * PLANT_SPRITE_WIDTH,
        0,
        PLANT_SPRITE_WIDTH,
        PLANT_SPRITE_HEIGHT,
    };

    Rectangle dest = {
        origin.x,
        origin.y,
        PLANT_SPRITE_WIDTH * WORLD_SCALE,
        PLANT_SPRITE_HEIGHT * WORLD_SCALE,
    };

    Vector2 pivot = {dest.width / 2, dest.height};

    DrawTexturePro(plantAtlas, source, dest, pivot, 0, WHITE);
}
