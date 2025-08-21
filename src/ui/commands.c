#include "../entity/garden.h"

void command_focusNextPlanter(Garden *garden) {
    if (garden->selectedPlanter == -1 || garden->selectedPlanter == garden->plantersCount - 1) {
        garden->selectedPlanter = 0;
    } else {
        garden->selectedPlanter = garden->selectedPlanter + 1;
    }
}

void command_focusPreviousPlanter(Garden *garden) {
    if (garden->selectedPlanter == -1 || garden->selectedPlanter == 0) {
        garden->selectedPlanter = garden->plantersCount - 1;
    } else {
        garden->selectedPlanter = garden->selectedPlanter - 1;
    }
}

void command_addPlant(Garden *garden) {
    if (garden->selectedPlanter == -1) {
        return;
    }

    planter_addPlant(&garden->planters[garden->selectedPlanter]);
}

void command_removePlant(Garden *garden) {
    if (garden->selectedPlanter == -1) {
        return;
    }

    planter_removePlant(&garden->planters[garden->selectedPlanter]);
}

void command_irrigate(Garden *garden) {
    if (garden->selectedPlanter == -1) {
        return;
    }

    plant_irrigate(&garden->planters[garden->selectedPlanter].plant);
}

void command_feed(Garden *garden) {
    if (garden->selectedPlanter == -1) {
        return;
    }

    plant_feed(&garden->planters[garden->selectedPlanter].plant);
}
