#pragma once

#include "../entity/garden.h"

#define INPUT_MAP_CAPACITY 16

typedef void (*CommandFunction)(Garden *);

typedef struct {
    CommandFunction command;
    int key;
} CommandInputPair;

typedef struct {
    CommandInputPair registeredCommands[INPUT_MAP_CAPACITY];
    int registeredCommandsCount;
} InputMap;

void inputmap_init(InputMap *inputMap);
void inputmap_processInput(InputMap *inputMap, Garden *garden);
