#pragma once

#include "../pch.h"

#include "aml-psdk/game_sa/engine/Sprite2d.h"

#include "map"
#include <cstddef>
#include <string>

struct RwTexture;

struct MenuTexture
{
    CSprite2d sprite;

    MenuTexture()
    {
        sprite.m_pTexture = nullptr;

        LeakUtils::RegisterItem("MenuTexture");
    }

    ~MenuTexture()
    {
        sprite.m_pTexture = nullptr;

        LeakUtils::FreeItem("MenuTexture");
    }
};

inline std::map<std::string, RwTexture *> g_texturesCache;

RwTexture *LoadRwTextureFromFile(std::string file, std::string textureName, bool flipHorizontal, CRGBA replaceColor);
RwTexture *LoadRwTextureFromFileAndCache(
    std::string file, std::string textureName, bool flipHorizontal, CRGBA replaceColor);

MenuTexture *CreateMenuTexture(std::string file, std::string textureName, bool flipHorizontal, CRGBA replaceColor);

void ExecuteWhenTexturesCanBeCreated(std::function<void()> fn);
void ProcessTexturesCallbacks();