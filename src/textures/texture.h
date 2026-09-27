#pragma once

#include "aml-psdk/game_sa/engine/Sprite2d.h"
#include "menu/menu.h"

class Texture : public ITexture
{
  public:
    CSprite2d sprite;

    // Texture(std::string imagePath);
    Texture(std::string imagePath, std::string textureName, bool flipHorizontal, CRGBA replaceColor);
    ~Texture();
};