#include "plant.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "raylib.h"
#include <assert.h>
#include <stdlib.h>

#define PLANT_STATE_COUNT 6

#define PLANT_STATUS_LEVEL_COUNT 4

typedef struct {
    /// 0 - 5
    int waterResiliece;
    int nutrientResiliece;
    PlantWaterLevel preferedWaterLevel;
    PlantNutrientsLevel preferedNutrientsLevel;
} PlantPreferences;

static const float PLANT_TICKS_PER_SECOND = 1.0f;
static const float PLANT_TICK_TIME = 1.0f / PLANT_TICKS_PER_SECOND;

int minmax(int min, int max) {
    if (max < min) {
        return max;
    }

    return min;
}

int clamp(int min, int max, int value) {
    if (min > value) {
        return min;
    }

    if (max < value) {
        return max;
    }

    return value;
}

int plant_getStatLevel(int statValue) {
    int x = 100 / PLANT_STATUS_LEVEL_COUNT;

    // int level = ((statValue + x - 1) / x) - 1;
    int level = statValue / x;
    int rest = statValue % x;
    if (rest > 0) {
        level++;
    }

    return level;
}

void plant_init(Plant *p, enum PlantType type) {
    p->type = type;
    p->mediumHydration = 0;
    p->mediumNutrition = 100;
    p->hydration = 50;
    p->nutrition = 0;
    p->health = 80;
    p->timeSinceLastTick = 0;

    switch (type) {
    case PLANT_TYPE_CRASSULA_OVATA:
        p->scientificName = "Crassula Ovata";
        p->name = "Jade plant";
        p->altName = "Lucky plant";

        p->overWateredResiliece = 0;
        // succulents prefer dry "stress"
        p->underWateredResiliece = 2;
        p->overNutritionResiliece = 0;
        p->underNutritionResiliece = 0;
        p->optimalWaterLevel = PLANT_WATER_LEVEL_DRY;
        p->optimalNutrientsLevel = PLANT_NUTRIENT_LEVEL_3;
        break;

    case PLANT_TYPE_SENECIO_ROWLEYANUS:
        p->overWateredResiliece = 0;
        // succulents prefer dry "stress"
        p->underWateredResiliece = 2;
        p->overNutritionResiliece = 0;
        p->underNutritionResiliece = 0;
        p->optimalWaterLevel = PLANT_WATER_LEVEL_MOIST;
        p->optimalNutrientsLevel = PLANT_NUTRIENT_LEVEL_3;

        p->scientificName = "Senecio Rowleyanus";
        p->name = "String of pearls";
        p->altName = "String of beads";
        break;

    case PLANT_TYPE_COUNT:
        assert(false);
        break;
    }
}

void plant_irrigate(Plant *p) {
    p->mediumHydration += 10;

    if (p->mediumHydration >= 100) {
        p->mediumHydration = 100;
    }
}

void plant_feed(Plant *p) {
    p->mediumNutrition += 10;

    if (p->mediumNutrition >= 100) {
        p->mediumNutrition = 100;
    }
}

void plant_update(Plant *plant, float deltaTime) {
    plant->timeSinceLastTick += deltaTime;

    if (plant->timeSinceLastTick >= PLANT_TICK_TIME) {
        // don't set to 0 but subtract in case timer > PLANT_TICK_TIME
        plant->timeSinceLastTick -= PLANT_TICK_TIME;
        // ------------------
        // Health changes

        int hidrationLevel = plant_getStatLevel(plant->hydration);
        int plantWaterLevelDistanceFromOptimal = plant->optimalWaterLevel - hidrationLevel;

        int waterHealthDelta = 0;

        bool waterStressedButResilient =
            (plantWaterLevelDistanceFromOptimal < 0 && plant->underWateredResiliece > 0) ||
            (plantWaterLevelDistanceFromOptimal > 0 && plant->overWateredResiliece > 0);

        switch (abs(plantWaterLevelDistanceFromOptimal)) {
        case 0:
            waterHealthDelta = 5;
            break;

        case 1:
            if (waterStressedButResilient) {
                waterHealthDelta = 3;
            } else {
                waterHealthDelta = 2;
            }

            break;
        case 2:
            if (waterStressedButResilient) {
                waterHealthDelta = 0;
            } else {
                waterHealthDelta = -1;
            }

            break;
        case 3:
            if (waterStressedButResilient) {
                waterHealthDelta = -2;
            } else {
                waterHealthDelta = -3;
            }

            break;
        case 4:
            if (waterStressedButResilient) {
                waterHealthDelta = -4;
            } else {
                waterHealthDelta = -6;
            }
            break;
        }

        plant->health += waterHealthDelta;

        int nutritionLevel = plant_getStatLevel(plant->nutrition);
        int plantNutrientsLevelDistanceFromOptimal = plant->optimalNutrientsLevel - nutritionLevel;

        int nutrientsHealthDelta = 0;

        bool nutrientStressedButResilient =
            (plantNutrientsLevelDistanceFromOptimal < 0 && plant->underNutritionResiliece > 0) ||
            (plantNutrientsLevelDistanceFromOptimal > 0 && plant->overNutritionResiliece > 0);

        switch (abs(plantNutrientsLevelDistanceFromOptimal)) {
        case 0:
            nutrientsHealthDelta = 3;
            break;

        case 1:
            nutrientsHealthDelta = 2;
            break;
        case 2:
            if (nutrientStressedButResilient) {
                nutrientsHealthDelta = 0;
            } else {
                nutrientsHealthDelta = -1;
            }

            break;
        case 3:
            if (nutrientStressedButResilient) {
                nutrientsHealthDelta = -2;
            } else {
                nutrientsHealthDelta = -3;
            }

            break;
        case 4:
            if (nutrientStressedButResilient) {
                nutrientsHealthDelta = -4;
            } else {
                nutrientsHealthDelta = -6;
            }
            break;
        }

        plant->health += nutrientsHealthDelta;
        plant->nutrition -= abs(nutrientsHealthDelta * 2);

        // ------------------
        // Usage of resources

        if (nutritionLevel > plant_getStatLevel(plant->mediumNutrition)) {
            plant->nutrition -= plant->mediumNutrition / 10;
        } else {
            plant->nutrition += plant->mediumNutrition / 10;
        }

        if (hidrationLevel < plant_getStatLevel(plant->mediumHydration)) {
            plant->hydration -= plant->mediumHydration / 10;
        } else {
            plant->hydration += plant->mediumHydration / 10;
        }

        // more water = more drainage. Eventually affected by soil type?
        int mediumHydrationLevel = plant_getStatLevel(plant->mediumHydration);
        switch (mediumHydrationLevel) {
        case 4:
            plant->mediumHydration *= 0.95f;
            break;
        case 3:
            plant->mediumHydration *= 0.97f;
            break;
        case 2:
        case 1:
            plant->mediumHydration *= 0.99f;
            break;
        case 0:
            break;
        }

        plant->mediumNutrition -= plant->mediumNutrition / 10 / 2;

        plant->health = clamp(0, 100, plant->health);
        plant->hydration = clamp(0, 100, plant->hydration);
        plant->nutrition = clamp(0, 100, plant->nutrition);
        plant->mediumHydration = clamp(0, 100, plant->mediumHydration);
        plant->mediumNutrition = clamp(0, 100, plant->mediumNutrition);
    }
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
