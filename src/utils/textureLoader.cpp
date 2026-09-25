#include "textureLoader.h"
#include "mod/logger.h"
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external_libs/stb/stb_image.h"
// #include "stb/stb_image_write.h"

#include <aml-psdk/game_sa/plugin.h>
#include <aml-psdk/renderware/RwImage.h>
#include <aml-psdk/renderware/RwRaster.h>
#include <aml-psdk/renderware/RwTexture.h>

RwTexture *LoadRwTextureFromFile(std::string file, std::string textureName, bool flipHorizontal, CRGBA replaceColor)
{
    logger->Info("Loading texture: %s", file.c_str());

    int width, height, channels;
    unsigned char *data = stbi_load(file.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!data)
    {
        LOGE("Failed to load image: %s", file.c_str());
        return nullptr;
    }

    RwRaster *raster = RwRasterCreate(width, height, 32, (RwInt32)rwRASTERTYPETEXTURE | (RwInt32)rwRASTERFORMAT8888);

    if (!raster)
    {
        logger->Error("Failed to create raster");
        stbi_image_free(data);
        return nullptr;
    }

    RwUInt8 *rasterPixels = (RwUInt8 *)RwRasterLock(raster, 0, rwRASTERLOCKWRITE);

    if (!rasterPixels)
    {
        logger->Error("Failed to lock raster");
        RwRasterDestroy(raster);
        stbi_image_free(data);
        return nullptr;
    }

    for (int y = 0; y < height; ++y)
    {
        int sourceY = flipHorizontal ? (height - 1 - y) : y;

        for (int x = 0; x < width; ++x)
        {
            int sourceIndex = (sourceY * width + x) * 4;
            int targetIndex = (y * width + x) * 4;

            unsigned char r = data[sourceIndex + 0];
            unsigned char g = data[sourceIndex + 1];
            unsigned char b = data[sourceIndex + 2];
            unsigned char a = data[sourceIndex + 3];

            // Verifica se o pixel é cinza.
            int minChannel = std::min({r, g, b});
            int maxChannel = std::max({r, g, b});

            bool isGray = (maxChannel - minChannel) <= 5;
            bool shouldReplace = isGray && (replaceColor.r != 255 || replaceColor.g != 255 || replaceColor.b != 255);

            if (shouldReplace)
            {
                float brightness = (r + g + b) / (3.0f * 255.0f);

                r = (unsigned char)(replaceColor.r * brightness);
                g = (unsigned char)(replaceColor.g * brightness);
                b = (unsigned char)(replaceColor.b * brightness);
            }

            rasterPixels[targetIndex + 0] = r;
            rasterPixels[targetIndex + 1] = g;
            rasterPixels[targetIndex + 2] = b;
            rasterPixels[targetIndex + 3] = a;
        }
    }

    RwRasterUnlock(raster);

    RwTexture *texture = RwTextureCreate(raster);

    if (!texture)
    {
        logger->Error("Failed to create texture");
        RwRasterDestroy(raster);
        stbi_image_free(data);
        return nullptr;
    }

    memset(texture->name, 0, 32);
    textureName.copy(texture->name, 32);

    stbi_image_free(data);

    logger->Info("Created texture: %s", file.c_str());

    return texture;
}

RwTexture *LoadRwTextureFromFileAndCache(
    std::string file, std::string textureName, bool flipHorizontal, CRGBA replaceColor)
{
    auto it = g_texturesCache.find(file);

    if (it != g_texturesCache.end())
        return it->second;

    RwTexture *texture = LoadRwTextureFromFile(file, textureName, flipHorizontal, replaceColor);

    if (texture)
        g_texturesCache[file] = texture;

    logger->Info("Textures cached: %d", g_texturesCache.size());

    return texture;
}

MenuTexture *CreateMenuTexture(std::string file, std::string textureName, bool flipHorizontal, CRGBA replaceColor)
{
    auto rwtexture = LoadRwTextureFromFileAndCache(file, "button", flipHorizontal, replaceColor);

    MenuTexture *texture = new MenuTexture();
    texture->sprite.m_pTexture = rwtexture;

    return texture;
}

std::vector<std::function<void()>> _callbacks;

bool CanCreateTextures() { return g_framesDrawn > 0; }

void ExecuteWhenTexturesCanBeCreated(std::function<void()> fn)
{
    if (CanCreateTextures())
    {
        fn();
        return;
    }

    _callbacks.push_back(fn);
}

void ProcessTexturesCallbacks()
{
    if (CanCreateTextures())
    {
        for (auto &callback : _callbacks)
            callback();

        _callbacks.clear();
    }
}