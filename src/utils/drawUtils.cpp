#include "drawUtils.h"

#include "../hooks.h"

#include "aml-psdk/game_sa/engine/Sprite2d.h"
#include "aml-psdk/gta_base/RGBA.h"

#include "aml-psdk/gta_base/Vector.h"
#include "mod/logger.h"

IResolution osResolution = {0, 0};
IResolution baseResolution = {2400, 1080};

IFont DefaultFont;
IFont CurrentFont = DefaultFont;

char s_buffer[0xFF];
unsigned short *gxt_text = new unsigned short[0xFF];

IResolution DrawUtils::GetBaseResolution() { return baseResolution; }

IFont *DrawUtils::GetCurrentFont() { return &CurrentFont; }

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

void DrawUtils::DrawText(
    std::string text, CVector2D position, IFont &font, CVector2D scale, bool clicked, float opacity)
{
    position.x = MapWidthToOS(position.x);
    position.y = MapHeightToOS(position.y);

    // quanto maior o magic number, mais pra baixo o texto fica
    float lineHeight = font.size * 22.0f;

    position.y -= MapHeightToOS(lineHeight * scale.y) / 2;

    float scaleX = font.size * (float)osResolution.width / (float)baseResolution.width;
    float scaleY = font.size * (float)osResolution.height / (float)baseResolution.height;

    scaleX *= scale.x;
    scaleY *= scale.y;

    auto fontColor = clicked ? font.clickedColor : font.color;

    fontColor.a = (unsigned char)((float)fontColor.a * opacity);

    CRGBA color = CRGBA(fontColor.r, fontColor.g, fontColor.b, fontColor.a);

    FontSetOrientation(font.align);
    FontSetColor(&color);
    FontSetBackground(false, false);
    FontSetWrapx(3000.0f);
    FontSetScale(scaleX, scaleY);
    FontSetStyle(font.style);
    FontSetProportional(true);
    FontSetDropShadowPosition(1);
    FontSetDropColor(&COLOR_BLACK);

    sprintf(s_buffer, "%s", text.c_str());
    AsciiToGxtChar(s_buffer, gxt_text);
    FontPrintString(position.x, position.y, gxt_text);

    RenderFontBuffer();
}

void DrawUtils::DrawText(std::string text, CVector2D position, CRGBA color)
{
    auto newFont = CurrentFont;
    newFont.color = color;

    DrawText(text, position, newFont, CVector2D(1, 1), false, 1);
}

void DrawUtils::DrawSprite(CSprite2d *sprite, CVector2D position, CVector2D size, CRGBA color)
{
    if (sprite->m_pTexture == nullptr)
    {
        // logger->Error("DrawSprite: texture is null");
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