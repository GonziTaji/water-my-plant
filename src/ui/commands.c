#include "../entity/garden.h"

void command_addPlanter(Garden *garden) {
    if (garden->tileSelected == -1 || garden->tiles[garden->tileSelected].hasPlanter) {
        return;
    }

    garden->tiles[garden->tileSelected].hasPlanter = true;
    planter_init(&garden->tiles[garden->tileSelected].planter);
}

void command_removePlanter(Garden *garden) {
    if (garden->tileSelected == -1 || !garden->tiles[garden->tileSelected].hasPlanter) {
        return;
    }

    garden->tiles[garden->tileSelected].hasPlanter = false;
}

void command_focusNextTile(Garden *garden) {
    if (garden->tileSelected == -1 || garden->tileSelected == garden->tilesCount - 1) {
        garden->tileSelected = 0;
    } else {
        garden->tileSelected = garden->tileSelected + 1;
    }
}

void command_focusPreviousTile(Garden *garden) {
    if (garden->tileSelected == -1 || garden->tileSelected == 0) {
        garden->tileSelected = garden->tilesCount - 1;
    } else {
        garden->tileSelected = garden->tileSelected - 1;
    }
}

void command_addPlant(Garden *garden) {
    if (garden->tileSelected == -1 || !garden->tiles[garden->tileSelected].hasPlanter) {
        return;
    }

    planter_addPlant(&garden->tiles[garden->tileSelected].planter);
}

void command_removePlant(Garden *garden) {
    if (garden->tileSelected == -1 || !garden->tiles[garden->tileSelected].hasPlanter ||
        !garden->tiles[garden->tileSelected].planter.hasPlant) {
        return;
    }

    planter_removePlant(&garden->tiles[garden->tileSelected].planter);
}

void command_irrigate(Garden *garden) {
    if (garden->tileSelected == -1) {
        return;
    }

    plant_irrigate(&garden->tiles[garden->tileSelected].planter.plant);
}

void command_feed(Garden *garden) {
    if (garden->tileSelected == -1) {
        return;
    }

    plant_feed(&garden->tiles[garden->tileSelected].planter.plant);
}
