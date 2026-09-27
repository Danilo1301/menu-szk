#pragma once

#include "../pch.h"

#include "aml-psdk/game_sa/engine/Sprite2d.h"

#include "map"
#include <cstddef>
#include <string>

struct RwTexture;

RwTexture *LoadRwTextureFromFile(std::string file, std::string textureName, bool flipHorizontal, CRGBA replaceColor);

RwTexture *LoadRwTextureFromFileAndCache(
    std::string file, std::string textureName, bool flipHorizontal, CRGBA replaceColor);

void ExecuteWhenTexturesCanBeCreated(std::function<void()> fn);
void ProcessTexturesCallbacks();