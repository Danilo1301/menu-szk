#include "texture.h"
#include "../utils/textureLoader.h"
#include "aml-psdk/gta_base/RGBA.h"
#include "src/utils/textureLoader.h"

// Texture::Texture(std::string imagePath)
// {
//     texture = LoadRwTextureFromFileAndCache(imagePath, "texture", false, COLOR_WHITE);
//     sprite.m_pTexture = texture;
// }

Texture::Texture(std::string imagePath, std::string textureName, bool flipHorizontal, CRGBA replaceColor)
{
    sprite.m_pTexture = nullptr;

    ExecuteWhenTexturesCanBeCreated(
        [this, imagePath, textureName, flipHorizontal, replaceColor]()
        {
            texture = LoadRwTextureFromFileAndCache(imagePath, textureName, flipHorizontal, replaceColor);
            sprite.m_pTexture = texture;
        });
}

Texture::~Texture()
{
    sprite.m_pTexture = nullptr;
    texture = nullptr;
}