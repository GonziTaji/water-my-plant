#include "plant.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "raylib.h"
#include <assert.h>

#define PLANT_STATE_COUNT 6

static const float PLANT_TICKS_PER_SECOND = 0.5f;
static const float PLANT_TICK_TIME = 1.0f / PLANT_TICKS_PER_SECOND;

void plant_init(Plant *p, enum PlantType type) {
    p->type = type;
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
}

PlantInfo plant_getInfo(enum PlantType type) {
    PlantInfo pi;

    switch (type) {
    case PLANT_TYPE_CRASSULA_OVATA:
        pi.scientificName = "Crassula Ovata";
        pi.name = "Jade plant";
        pi.altName = "Lucky plant";
        break;

    case PLANT_TYPE_SENECIO_ROWLEYANUS:
        pi.scientificName = "Senecio Rowleyanus";
        pi.name = "String of pearls";
        pi.altName = "String of beads";
        break;

    case PLANT_TYPE_COUNT:
        assert(false);
        break;
    }

    return pi;
}

Rectangle plant_getSpriteSourceRect(enum PlantType type, int health) {
    int spriteWidth;
    int spriteHeight;
    // Position in the sprite atlas
    int spriteOriginX;
    int spriteOriginY;

    switch (type) {
    case PLANT_TYPE_CRASSULA_OVATA:
        spriteWidth = 64;
        spriteHeight = 64;
        spriteOriginY = 0;
        break;

    case PLANT_TYPE_SENECIO_ROWLEYANUS:
        spriteWidth = 64;
        spriteHeight = 64;
        spriteOriginY = 0;
        break;

    case PLANT_TYPE_COUNT:
        assert(false);
        break;
    }

    int spriteIndex;
    if (health > 70) {
        spriteIndex = 0;
    } else if (health > 50) {
        spriteIndex = 1;
    } else if (health > 40) {
        spriteIndex = 2;
    } else if (health > 20) {
        spriteIndex = 3;
    } else if (health > 10) {
        spriteIndex = 4;
    } else {
        spriteIndex = 5;
    }

    spriteOriginX = spriteIndex * spriteWidth;

    return (Rectangle){
        spriteOriginX,
        spriteOriginY,
        spriteWidth,
        spriteHeight,
    };
}

/// The plant is drawn with the center of its base at the origin
void plant_draw(Plant *plant, Vector2 origin) {
    Rectangle source = plant_getSpriteSourceRect(plant->type, plant->health);

    Rectangle dest = {
        origin.x,
        origin.y,
        source.width * WORLD_SCALE,
        source.height * WORLD_SCALE,
    };

    Vector2 pivot = {dest.width / 2, dest.height};

    DrawTexturePro(plantAtlas, source, dest, pivot, 0, WHITE);
}
