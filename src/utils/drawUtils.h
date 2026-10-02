#pragma once

#include "../pch.h"

#include "aml-psdk/gta_base/RGBA.h"
#include "aml-psdk/gta_base/Rect.h"
#include "aml-psdk/gta_base/Vector.h"
#include "menuSZK/imenuSZK.h"

struct RwTexture;
struct CSprite2d;
class Texture;

struct IResolution
{
    int width;
    int height;
};

class DrawUtils
{
public:
    static void TryFindResolution();

    static IResolution GetBaseResolution();
    static IFontStyle* GetCurrentFont();

    static void DrawRect_original(CRect rect, CRGBA color);
    static void DrawRect(CVector2D position, CVector2D size, CRGBA color);

    static void DrawText(const std::string& text, CVector2D position, IFontStyle& fontStyle);
    static void DrawText(std::string text, CVector2D position, CRGBA color);

    static void DrawSprite_original(CSprite2d* sprite, CRect rect, CRGBA color);
    static void DrawSprite(CSprite2d* sprite, CVector2D position, CVector2D size, CRGBA color);
    static void DrawTexture(Texture* texture, CVector2D position, CVector2D size, CRGBA color);
    static void DrawTextureOnRadar(Texture* texture, CVector2D radarPosition, CVector2D size, CRGBA color);

    static float MapWidthToOS(float value);
    static float MapHeightToOS(float value);
    static float MapWidthFromOS(float value);
    static float MapHeightFromOS(float value);

    static CVector2D ConvertWorldToScreenCoords(CVector worldPosition, bool useMenuCoords);
};