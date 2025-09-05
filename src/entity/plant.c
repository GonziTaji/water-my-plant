
#include "plant.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "../utils/utils.h"
#include "raylib.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#define PLANT_STATE_COUNT 6
#define PLANT_TICKS_PER_SECOND 1.0f

typedef struct {
    /// 0 - 5
    int waterResiliece;
    int nutrientResiliece;
    PlantWaterLevel preferedWaterLevel;
    PlantNutrientsLevel preferedNutrientsLevel;
} PlantPreferences;

int minmax(int min, int max) {
    if (max < min) {
        return max;
    }

    return min;
}

float absf(float f) {
    return f > 0 ? f : -f;
}

int plant_getMaxValueForLevel(int level) {
    int pointsPerLevel = 100.0f / PLANT_STATUS_LEVEL_COUNT;

    // level at 1 point to be next level
    return (pointsPerLevel * (level + 1)) - 1;
}

int plant_getStatLevel(float statValue) {
    float x = 100.0f / PLANT_STATUS_LEVEL_COUNT;

    // int level = ((statValue + x - 1) / x) - 1;
    int level = utils_clampf(0, 100, statValue) / x;
    // int rest = (int)statValue % (int)x;
    // if (rest > 0) {
    //     level++;
    // }

    return level;
}

void plant_init(Plant *p, enum PlantType type) {
    p->type = type;
    p->mediumHydration = 0;
    p->mediumNutrition = 0;
    p->hydration = plant_getMaxValueForLevel(2);
    p->nutrition = plant_getMaxValueForLevel(2);
    p->health = 80;
    p->timeSinceLastTick = 0;
    p->ticksCount = 0;
    p->overWateredResiliece = false;
    p->underWateredResiliece = false;
    p->overNutritionResiliece = false;
    p->underNutritionResiliece = false;

    switch (type) {
    case PLANT_TYPE_CRASSULA_OVATA:
        p->scientificName = "Crassula Ovata";
        p->name = "Jade plant";
        p->altName = "Lucky plant";

        // Succulents prefer dry "stress"
        p->underWateredResiliece = true;
        p->optimalWaterLevel = PLANT_WATER_LEVEL_DRY;
        p->optimalNutrientsLevel = PLANT_NUTRIENT_LEVEL_3;
        break;

    case PLANT_TYPE_SENECIO_ROWLEYANUS:
        p->scientificName = "Senecio Rowleyanus";
        p->name = "String of pearls";
        p->altName = "String of beads";

        // Succulents prefer dry "stress"
        p->underWateredResiliece = true;
        p->optimalWaterLevel = PLANT_WATER_LEVEL_MOIST;
        p->optimalNutrientsLevel = PLANT_NUTRIENT_LEVEL_3;
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

float exponential(float initial, float rate, float x) {
    return initial * pow((1 + rate), x);
}

void plant_update(Plant *plant, float deltaTime) {
    deltaTime *= PLANT_TICKS_PER_SECOND;

    float healthChange = 0;

    const int hydrationLevel = plant_getStatLevel(plant->hydration);
    const int optimalHydrationLevel = 2;

    const int hydrationLevelDistanceFromOptimal = abs(optimalHydrationLevel - hydrationLevel);

    // Health change based on hydration
    switch (hydrationLevelDistanceFromOptimal) {
    case 0:
        healthChange += 1;
        break;
    case 2:
        healthChange -= 2;
        break;
    }

    const int nutritionLevel = plant_getStatLevel(plant->nutrition);
    const int optimalNutritionLevel = 2;

    int nutritionLevelDistanceFromOptimal = abs(optimalNutritionLevel - nutritionLevel);

    // Health change based on hydration
    switch (nutritionLevelDistanceFromOptimal) {
    case 0:
        healthChange += 1;
        break;
    case 2:
        healthChange -= 2;
        break;
    }

    plant->health += healthChange * deltaTime;

    // Hydration change based on hydration medium
    const int mediumHydrationLevel = plant_getStatLevel(plant->mediumHydration);
    const int mediumWaterLevelDistanceFromOptimal = plant->optimalWaterLevel - mediumHydrationLevel;
    float hydrationChange = 0;

    if (mediumWaterLevelDistanceFromOptimal == 0) {
        // in favorite medium level

        // if it likes saturated medium it can never be over watered
        if (plant->optimalWaterLevel == 4) {
            hydrationChange = 1;

            if (nutritionLevel > 2) {
                plant->hydration = plant_getMaxValueForLevel(2);
            }
        } else if (plant->hydration < plant->mediumHydration) {
            hydrationChange = 2;
        } else {
            hydrationChange = -2;
        }
    } else if (mediumHydrationLevel == 0 && plant->hydration < plant->mediumHydration) {
        // when hydration in plant and medium are 0, and the medium is irrigated, hydration
        // should go up
        hydrationChange += 1;
    } else {
        hydrationChange = -mediumWaterLevelDistanceFromOptimal;
    }

    plant->hydration += hydrationChange * deltaTime;

    // Medium hydration change based on own level and plant hydration change
    float hydrationLoss = absf(hydrationChange) * 0.5f;
    // Drainage for higher hydration levels
    if (mediumHydrationLevel > 2) {
        hydrationLoss += (mediumHydrationLevel - 2);
    }

    plant->mediumHydration -= hydrationLoss * deltaTime;

    // Hydration change based on hydration medium
    float nutritionChange = 0;
    const int mediumNutritionLevel = plant_getStatLevel(plant->mediumNutrition);
    const int healthLevel = plant_getStatLevel(plant->health);

    if (plant->mediumNutrition == 0) {
        nutritionChange = -healthLevel * 0.5f;
    } else {
        // less healthy => less nutrients consumed
        float mediumNutrientLevelFactor = (mediumNutritionLevel) * 0.5f;

        int healthLevelFactor = (healthLevel * 0.5f) + 1;

        nutritionChange = healthLevelFactor + mediumNutrientLevelFactor;
        nutritionChange = minmax(nutritionChange, plant->mediumNutrition);
    }

    plant->nutrition += nutritionChange * deltaTime;
    plant->mediumNutrition -= absf(nutritionChange) * 0.5f * deltaTime;

    // clamp stat values
    plant->health = utils_clampf(0, 100, plant->health);
    plant->hydration = utils_clampf(0, 100, plant->hydration);
    plant->nutrition = utils_clampf(0, 100, plant->nutrition);
    plant->mediumHydration = utils_clampf(0, 100, plant->mediumHydration);
    plant->mediumNutrition = utils_clampf(0, 100, plant->mediumNutrition);
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
void plant_draw(Plant *plant, Vector2 origin, float scale) {
    Rectangle source = plant_getSpriteSourceRect(plant->type, plant->health);

    Rectangle dest = {
        origin.x,
        origin.y,
        source.width * scale,
        source.height * scale,
    };

    Vector2 pivot = {dest.width / 2, dest.height};

    DrawTexturePro(plantAtlas, source, dest, pivot, 0, WHITE);
}
