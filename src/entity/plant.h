#pragma once
#include <raylib.h>

#define PLANT_SPRITE_WIDTH 64
#define PLANT_SPRITE_HEIGHT 64

enum PLANT_TYPE {
    PLANT_TYPE_A,
    PLANT_TYPE_B,
    PLANT_TYPE_C,
};

typedef struct {
    enum PLANT_TYPE type;
    int water;
    int nutrients;
    int health;
    float timeSinceLastTick;
    int healthStatusIndex;
} Plant;

void plant_init(Plant *p, enum PLANT_TYPE type);
void plant_irrigate(Plant *p);
void plant_feed(Plant *p);
void plant_printStats(Plant *p);
void plant_update(Plant *plant, float deltaTime);
void plant_draw(Plant *plant, Vector2 origin);
