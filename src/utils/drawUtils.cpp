#include "drawUtils.h"

#include "../hooks.h"

#include "aml-psdk/game_sa/engine/Sprite2d.h"
#include "aml-psdk/gta_base/RGBA.h"

#include "../textures/texture.h"
#include "aml-psdk/gta_base/Vector.h"
#include "menu/menu.h"
#include "mod/logger.h"
#include "src/pch.h"

IResolution osResolution = {0, 0};
IResolution baseResolution = {2400, 1080};

IFontStyle DefaultFont;
IFontStyle CurrentFont = DefaultFont;

char s_buffer[0xFF];
unsigned short *gxt_text = new unsigned short[0xFF];

IResolution DrawUtils::GetBaseResolution() { return baseResolution; }

IFontStyle *DrawUtils::GetCurrentFont() { return &CurrentFont; }

void DrawUtils::TryFindResolution()
{
    int width = OS_ScreenGetWidth();
    int height = OS_ScreenGetHeight();

    if (width != osResolution.width || height != osResolution.height)
    {
        osResolution.width = width;
        osResolution.height = height;

        logger->Info("screen size changed!");
        logger->Info("width: %d", osResolution.width);
        logger->Info("height: %d", osResolution.height);
    }
}

void DrawUtils::DrawRect_original(CRect rect, CRGBA color) { CSprite2d_DrawRect(rect, color); }

void DrawUtils::DrawRect(CVector2D position, CVector2D size, CRGBA color)
{
    float mappedX = MapWidthToOS(position.x);
    float mappedY = MapHeightToOS(position.y);

    float mappedW = MapWidthToOS(size.x);
    float mappedH = MapHeightToOS(size.y);

    CRect rect(mappedX, mappedY, mappedX + mappedW, mappedY + mappedH);
    DrawRect_original(rect, color);
}

void DrawUtils::DrawText(const std::string &text, CVector2D position, IFontStyle &fontStyle)
{
    position.x = MapWidthToOS(position.x);
    position.y = MapHeightToOS(position.y);

    const float lineHeight = fontStyle.size * 22.0f;

    position.y -= MapHeightToOS(lineHeight * fontStyle.scale.y) / 2;

    const float scaleX = fontStyle.size * ((float)osResolution.width / (float)baseResolution.width) * fontStyle.scale.x;

    const float scaleY =
        fontStyle.size * ((float)osResolution.height / (float)baseResolution.height) * fontStyle.scale.y;

    CRGBA color = fontStyle.color;
    color.a = static_cast<unsigned char>(color.a * fontStyle.opacity);

    FontSetOrientation(fontStyle.align);
    FontSetColor(&color);
    FontSetBackground(false, false);
    FontSetWrapx(3000.0f);
    FontSetScale(scaleX, scaleY);
    FontSetStyle(fontStyle.style);
    FontSetProportional(true);
    FontSetDropShadowPosition(fontStyle.dropShadowPosition);
    FontSetDropColor(&fontStyle.dropColor);

    sprintf(s_buffer, "%s", text.c_str());
    AsciiToGxtChar(s_buffer, gxt_text);
    FontPrintString(position.x, position.y, gxt_text);

    RenderFontBuffer();
}

void DrawUtils::DrawText(std::string text, CVector2D position, CRGBA color)
{
    auto newFont = DefaultFont;
    newFont.color = color;

    DrawText(text, position, newFont);
}

void DrawUtils::DrawSprite(CSprite2d *sprite, CVector2D position, CVector2D size, CRGBA color)
{
    if (sprite->m_pTexture == nullptr)
    {
        LOGE("Called DrawSprite with a null texture");
        return;
    }

    float mappedX = MapWidthToOS(position.x);
    float mappedY = MapHeightToOS(position.y);

    float mappedW = MapWidthToOS(size.x);
    float mappedH = MapHeightToOS(size.y);

    // if (centered)
    // {
    //     mappedX -= mappedW * 0.5f;
    //     mappedY -= mappedH * 0.5f;
    // }

    CSprite2d_DrawSprite(sprite,
        CRect(mappedX,         // esquerda
            mappedY,           // topo
            mappedX + mappedW, // direita
            mappedY + mappedH  // fundo
            ),
        color);
}

void DrawUtils::DrawTexture(Texture *texture, CVector2D position, CVector2D size, CRGBA color)
{
    DrawSprite(&texture->sprite, position, size, color);
}

float DrawUtils::MapWidthToOS(float value)
{
    float mappedW = (value / baseResolution.width) * osResolution.width;
    return mappedW;
}

float DrawUtils::MapHeightToOS(float value)
{
    float mappedH = (value / baseResolution.height) * osResolution.height;
    return mappedH;
}

float DrawUtils::MapWidthFromOS(float value) { return (value / osResolution.width) * baseResolution.width; }

float DrawUtils::MapHeightFromOS(float value) { return (value / osResolution.height) * baseResolution.height; }