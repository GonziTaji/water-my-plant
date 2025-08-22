#pragma once

#include "../entity/garden.h"

void command_focusNextTile(Garden *garden);
void command_focusPreviousTile(Garden *garden);
void command_addPlanter(Garden *garden);
void command_removePlanter(Garden *garden);
void command_addPlant(Garden *garden);
void command_removePlant(Garden *garden);
void command_irrigate(Garden *garden);
void command_feed(Garden *garden);
