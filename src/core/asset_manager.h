#include <raylib.h>

// Change to a struct and a function to get assetManager instance if this grows
extern Texture2D plantAtlas;
extern Texture2D gardenTexture;
extern Texture2D planterTexture;
extern Font uiFont;

void assetManager_loadAssets();
void assetManager_unloadAssets();
