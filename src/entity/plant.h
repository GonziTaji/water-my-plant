#pragma once

typedef struct {
    int water;
    int nutrients;
    int health;
    float timeSinceLastTick;
} Plant;

void plant_init(Plant *p);
void plant_irrigate(Plant *p);
void plant_feed(Plant *p);
void plant_printStats(Plant *p);
void plant_update(Plant *plant, float deltaTime);
void plant_draw(Plant *plant);
