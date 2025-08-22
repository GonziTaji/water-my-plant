#pragma once
#include <raylib.h>

#define PLANT_SPRITE_WIDTH 64
#define PLANT_SPRITE_HEIGHT 64

typedef struct {
    int water;
    int nutrients;
    int health;
    float timeSinceLastTick;
    int healthStatusIndex;
} Plant;

void plant_loadTextures();
void plant_unloadTextures();
void plant_init(Plant *p);
void plant_irrigate(Plant *p);
void plant_feed(Plant *p);
void plant_printStats(Plant *p);
void plant_update(Plant *plant, float deltaTime);
void plant_draw(Plant *plant, Vector2 origin);
