#pragma once
#include <raylib.h>

#define PLANT_SPRITE_WIDTH 64
#define PLANT_SPRITE_HEIGHT 64
#define PLANT_STATUS_LEVEL_COUNT 5

enum PlantType {
    PLANT_TYPE_CRASSULA_OVATA,     // jade
    PLANT_TYPE_SENECIO_ROWLEYANUS, // string of pearls
    PLANT_TYPE_COUNT,
};

typedef enum {
    PLANT_WATER_LEVEL_DRY,
    PLANT_WATER_LEVEL_DAMP,
    PLANT_WATER_LEVEL_MOIST,
    PLANT_WATER_LEVEL_WET,
    PLANT_WATER_LEVEL_SATURATED,
} PlantWaterLevel;

typedef enum {
    PLANT_HEALTH_LEVEL_DEAD,
    PLANT_HEALTH_LEVEL_DYING,
    PLANT_HEALTH_LEVEL_STRESSED,
    PLANT_HEALTH_LEVEL_HEALTHY,
    PLANT_HEALTH_LEVEL_THRIVING,
} PlantHealthLevel;

typedef enum {
    PLANT_NUTRIENT_LEVEL_1,
    PLANT_NUTRIENT_LEVEL_2,
    PLANT_NUTRIENT_LEVEL_3,
    PLANT_NUTRIENT_LEVEL_4,
    PLANT_NUTRIENT_LEVEL_5,
} PlantNutrientsLevel;

typedef struct {
    enum PlantType type;
    const char *scientificName;
    const char *name;
    const char *altName;
    float mediumHydration;
    float mediumNutrition;
    float hydration;
    float nutrition;
    float health;
    float timeSinceLastTick;
    int ticksCount;
    bool overWateredResiliece;
    bool underWateredResiliece;
    bool overNutritionResiliece;
    bool underNutritionResiliece;
    PlantWaterLevel optimalWaterLevel;
    PlantNutrientsLevel optimalNutrientsLevel;
} Plant;

void plant_init(Plant *p, enum PlantType type);
void plant_irrigate(Plant *p);
void plant_feed(Plant *p);
void plant_update(Plant *plant, float deltaTime);
Rectangle plant_getSpriteSourceRect(enum PlantType type, int health);
void plant_draw(Plant *plant, Vector2 origin);
int plant_getStatLevel(float statValue);
