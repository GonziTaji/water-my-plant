#pragma once
#include <raylib.h>

#define PLANT_SPRITE_WIDTH 64
#define PLANT_SPRITE_HEIGHT 64

enum PlantType {
    PLANT_TYPE_CRASSULA_OVATA,     // jade
    PLANT_TYPE_SENECIO_ROWLEYANUS, // rosario
    PLANT_TYPE_COUNT,              // keep at the end
};

typedef struct {
    // TODO: add water needs, heat/sunlight tolerance, etc
    const char *scientificName;
    const char *name;
    const char *altName;
} PlantInfo;

typedef struct {
    enum PlantType type;
    int water;
    int nutrients;
    int health;
    float timeSinceLastTick;
} Plant;

void plant_init(Plant *p, enum PlantType type);
void plant_irrigate(Plant *p);
void plant_feed(Plant *p);
void plant_update(Plant *plant, float deltaTime);
PlantInfo plant_getInfo(enum PlantType type);
Rectangle plant_getSpriteSourceRect(enum PlantType type, int health);
void plant_draw(Plant *plant, Vector2 origin);
