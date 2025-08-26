
#include <raylib.h>
#include <stdio.h>

Texture2D plantAtlas;
Texture2D gardenTexture;
Texture2D planterTexture;
Font uiFont;

void assetManager_loadAssets() {
    char *fontUrl = "resources/fonts/friendlyscribbles/friendlyscribbles.ttf";
    uiFont = LoadFontEx(fontUrl, 64, NULL, 0);
    SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);

    plantAtlas = LoadTexture("resources/assets/plant1.png");

    gardenTexture = LoadTexture("resources/assets/floor.png");
    SetTextureFilter(gardenTexture, TEXTURE_FILTER_BILINEAR);

    planterTexture = LoadTexture("resources/assets/planter-1.png");
}

// if this is done when the game closes, is it really necesary?
void assetManager_unloadAssets() {
    UnloadFont(uiFont);
    UnloadTexture(plantAtlas);
    UnloadTexture(gardenTexture);
    UnloadTexture(planterTexture);
}
