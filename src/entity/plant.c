
#include "plant.h"
#include "../core/asset_manager.h"
#include "../game/constants.h"
#include "raylib.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#define PLANT_STATE_COUNT 6
#define PLANT_STATUS_LEVEL_COUNT 5
#define PLANT_TICKS_PER_SECOND 2.0f

typedef struct {
    /// 0 - 5
    int waterResiliece;
    int nutrientResiliece;
    PlantWaterLevel preferedWaterLevel;
    PlantNutrientsLevel preferedNutrientsLevel;
} PlantPreferences;

// static const float PLANT_TICKS_PER_SECOND = 4.0f;

int minmax(int min, int max) {
    if (max < min) {
        return max;
    }

    return min;
}

float clampf(float min, float max, float value) {
    if (min > value) {
        return min;
    }

    if (max < value) {
        return max;
    }

    return value;
}

float absf(float f) {
    return f > 0 ? f : -f;
}

int plant_getMaxValueForLevel(int level) {
    int pointsPerLevel = 100.0f / PLANT_STATUS_LEVEL_COUNT;

    // level at 1 point to be next level
    return (pointsPerLevel * level) + level - 1;
}

int plant_getStatLevel(float statValue) {
    float x = 100.0f / PLANT_STATUS_LEVEL_COUNT;

    // int level = ((statValue + x - 1) / x) - 1;
    int level = statValue / x;
    // int rest = (int)statValue % (int)x;
    // if (rest > 0) {
    //     level++;
    // }

    return level;
}

void plant_init(Plant *p, enum PlantType type) {
    p->type = type;
    p->mediumHydration = 40;
    p->mediumNutrition = 0;
    p->hydration = 40;
    p->nutrition = 0;
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
    plant->timeSinceLastTick += deltaTime;
    // float plantTickTime = 1 / PLANT_TICKS_PER_SECOND;

    if (true) {
        // if (plant->timeSinceLastTick >= plantTickTime) {
        // printf("PLANT TICKED\n");
        // don't set to 0 but subtract in case timer > PLANT_TICK_TIME
        // plant->timeSinceLastTick -= plantTickTime;
        // to 0 while developing because it adds to
        plant->timeSinceLastTick = 0;

        float healthChange = 0;

        int hydrationLevel = plant_getStatLevel(plant->hydration);
        int optimalHydrationLevel = 2;

        int hydrationLevelDistanceFromOptimal = abs(optimalHydrationLevel - hydrationLevel);

        // Health change based on hydration
        switch (hydrationLevelDistanceFromOptimal) {
        case 0:
            healthChange += 1;
            break;
        case 2:
            healthChange -= 2;
            break;
        }

        int nutritionLevel = plant_getStatLevel(plant->nutrition);
        int optimalNutritionLevel = 2;

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
        int mediumHydrationLevel = plant_getStatLevel(plant->mediumHydration);
        int mediumWaterLevelDistanceFromOptimal = plant->optimalWaterLevel - mediumHydrationLevel;
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
                hydrationChange = 1;
            } else {
                hydrationChange = -1;
            }
        } else if (mediumHydrationLevel == 0 && plant->hydration < plant->mediumHydration) {
            // when hydration in plant and medium are 0, and the medium is irrigated, hydration
            // should go up
            hydrationChange += 1;
        } else {
            hydrationChange = -mediumWaterLevelDistanceFromOptimal / 2.0f;
        }

        plant->hydration += hydrationChange * deltaTime;

        // Medium hydration change based on own level
        // Constant hydration loss
        int hydrationLoss = 1;
        // Drainage for higher hydration levels
        if (mediumHydrationLevel > 2) {
            hydrationLoss += (mediumHydrationLevel - 2);
        }

        plant->mediumHydration -= hydrationLoss * deltaTime;

        // Hydration change based on hydration medium
        float nutritionChange = 0;
        int mediumNutritionLevel = plant_getStatLevel(plant->mediumHydration);

        // TODO: consume nutrients based on how healthy it is
        // less healthy => less nutrients consumed
        int healthLevel = plant_getStatLevel(plant->health);

        int mediumNutrientLevelFactor = (mediumNutritionLevel - 2) * 0.5;

        int healthLevelFactor = (healthLevel / 2.0f) + 1;
        if (mediumNutrientLevelFactor <= 0) {
            healthLevelFactor *= -1;
        }

        nutritionChange = healthLevelFactor + mediumNutrientLevelFactor;

        // cannot consume more than available
        if (nutritionChange > 0) {
            nutritionChange = minmax(nutritionChange, plant->mediumNutrition);
        }

        plant->nutrition += nutritionChange * deltaTime;

        // constant for now
        plant->mediumNutrition -= absf(nutritionChange) * 2 * deltaTime;

        // clamp stat values
        plant->health = clampf(0, 100, plant->health);
        plant->hydration = clampf(0, 100, plant->hydration);
        plant->nutrition = clampf(0, 100, plant->nutrition);
        plant->mediumHydration = clampf(0, 100, plant->mediumHydration);
        plant->mediumNutrition = clampf(0, 100, plant->mediumNutrition);
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
