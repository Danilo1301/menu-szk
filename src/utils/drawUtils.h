#pragma once

#include "../pch.h"

#include "aml-psdk/gta_base/RGBA.h"
#include "aml-psdk/gta_base/Rect.h"

struct RwTexture;
struct CSprite2d;

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
    static IFont *GetCurrentFont();

    static void DrawRect_original(CRect rect, CRGBA color);
    static void DrawRect(CVector2D position, CVector2D size, CRGBA color);

    // static void DrawText_original(std::string text, CVector2D position);
    static void DrawText(
        std::string text, CVector2D position, IFont &font, CVector2D scale, bool clicked, float opacity);
    static void DrawText(std::string text, CVector2D position, CRGBA color);

    static void DrawSprite(CSprite2d *sprite, CVector2D position, CVector2D size, CRGBA color);

    static float MapWidthToOS(float value);
    static float MapHeightToOS(float value);
    static float MapWidthFromOS(float value);
    static float MapHeightFromOS(float value);
};