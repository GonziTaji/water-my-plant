#pragma once

#include "../entity/garden.h"

void command_focusNextPlanter(Garden *garden);
void command_focusPreviousPlanter(Garden *garden);
void command_addPlant(Garden *garden);
void command_removePlant(Garden *garden);
