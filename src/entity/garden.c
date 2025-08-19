
#include "garden.h"
#include "plant.h"
#include <math.h>
#include <raylib.h>

Texture2D texture;

double degToRad(float degrees) {
    return degrees * (M_PI / 180);
}

typedef struct {
    Vector2 vertices[4];
} RectVertices;

bool pointInsideVertices(Vector2 point, RectVertices rv) {
    int i, j;

    bool inside = false;

    for (i = 0, j = 3; i < 4; j = i++) {
        Vector2 vi = rv.vertices[i];
        Vector2 vj = rv.vertices[j];
        if (((vi.y > point.y) != (vj.y > point.y)) &&
            (point.x < (vj.x - vi.x) * (point.y - vi.y) / (vj.y - vi.y) + vi.x)) {
            inside = !inside;
        }
    }
    return inside;
}

Vector2 isoTransform(float x, float y) {
    // This can be written in a simpler way but it's like this so I understand what's going on.
    // Change the code if there are performance issues
    int sourceMatrix[2][1] = {
        {x},
        {y},
    };

    // Isometric view is usually a {45deg, 30deg} projection
    double multipMatrix[2][2] = {
        {cos(degToRad(30)), -cos(degToRad(30))},
        {sin(degToRad(30)), cos(degToRad(30))},
    };

    Vector2 resultMatrix = {
        sourceMatrix[0][0] * multipMatrix[0][0] + sourceMatrix[1][0] * multipMatrix[0][1],
        sourceMatrix[0][0] * multipMatrix[1][0] + sourceMatrix[1][0] * multipMatrix[1][1],
    };

    return resultMatrix;
}

RectVertices getPlanterIsoVertices(Rectangle rect) {
    RectVertices rv = {
        .vertices =
            {
                isoTransform(rect.x, rect.y),
                isoTransform(rect.x + rect.width, rect.y),
                isoTransform(rect.x + rect.width, rect.y + rect.height),
                isoTransform(rect.x, rect.y + rect.height),

            },
    };

    for (int i = 0; i < 4; i++) {
        rv.vertices[i].x += GARDEN_ORIGIN_X + PLANTER_SPACING_X;
        rv.vertices[i].y += GARDEN_ORIGIN_Y + PLANTER_SPACING_Y;
    }

    return rv;
}

Vector2 getPlanterCoordsFromIndex(int i) {
    return (Vector2){
        i == 0 ? 0 : i % GARDEN_COLS,
        i == 0 ? 0 : i / GARDEN_COLS,
    };
}

void garden_init(Garden *garden) {
    plant_loadTextures();

    garden->selectedPlanter = 0;
    garden->plantersCount = GARDEN_COLS * GARDEN_ROWS;

    texture = LoadTexture("resources/assets/balcony_floor.png");

    for (int i = 0; i < garden->plantersCount; i++) {
        int col = i % GARDEN_COLS;
        int row = i / GARDEN_COLS;

        float x = col * (PLANTER_WIDTH + PLANTER_SPACING_X);
        float y = row * (PLANTER_HEIGHT + PLANTER_SPACING_Y);

        planter_init(&garden->planters[i], (Rectangle){x, y, PLANTER_WIDTH, PLANTER_HEIGHT});
    }
}

void garden_update(Garden *garden, float deltaTime) {
    for (int i = 0; i < garden->plantersCount; i++) {
        if (garden->planters[i].hasPlant) {
            plant_update(&garden->planters[i].plant, deltaTime);
        }
    }
}

void garden_processClick(Garden *garden, const InputManager *input) {
    if (input->mouseButtonPressed[0]) {
        for (int i = 0; i < garden->plantersCount; i++) {

            if (pointInsideVertices(
                    input->mousePosVirtual, getPlanterIsoVertices(garden->planters[i].bounds))) {
                garden->selectedPlanter = i;
            }

            if (CheckCollisionPointRec(input->mousePosVirtual, garden->planters[i].bounds)) {
                garden->selectedPlanter = i;
            }
        }
    }
}

void garden_draw(Garden *garden) {
    DrawTexture(texture, 300, 130, WHITE);

    for (int i = 0; i < garden->plantersCount; i++) {
        Color planterBorderColor = garden->selectedPlanter == i ? BEIGE : BROWN;
        Rectangle bounds = garden->planters[i].bounds;
        DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, planterBorderColor);

        RectVertices rv = getPlanterIsoVertices(bounds);

        float radiusx = (rv.vertices[1].x - rv.vertices[3].x) / 2;
        float radiusy = (rv.vertices[2].y - rv.vertices[0].y) / 2;
        int centerx = rv.vertices[3].x + radiusx;
        int centery = rv.vertices[0].y + radiusy;

        DrawEllipse(centerx, centery, radiusx, radiusy, planterBorderColor);

        DrawLineEx(rv.vertices[0], rv.vertices[1], 2, RED);
        DrawLineEx(rv.vertices[1], rv.vertices[2], 2, GREEN);
        DrawLineEx(rv.vertices[2], rv.vertices[3], 2, BLACK);
        DrawLineEx(rv.vertices[3], rv.vertices[0], 2, YELLOW);

        if (garden->planters[i].hasPlant) {
            plant_draw(&garden->planters[i].plant, (Vector2){rv.vertices[0].x, rv.vertices[0].y});
            plant_draw(&garden->planters[i].plant, (Vector2){bounds.x, bounds.y});
        }
    }
}

void garden_unload() {
    UnloadTexture(texture);
    plant_unloadTextures();
}
