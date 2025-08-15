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
} KeyMap;

void keyMap_init(KeyMap *keyMap);
void keyMap_processInput(KeyMap *keyMap, Garden *garden);
