#include "radarBlip.h"
#include "../textures/texture.h"
#include <vector>

#include "../hooks.h"
#include "aml-psdk/gta_base/RGBA.h"
#include "aml-psdk/gta_base/Vector.h"
#include "mod/logger.h"
#include "src/pch.h"
#include "src/utils/drawUtils.h"
#include <aml-psdk/game_sa/engine/MobileMenu.h>

// https://github.com/AndroidModLoader/GTASA_CLEOPlus/blob/a23fbeb549e6a7cbd1205a04bfc56cb822964e85/cleoplus/radarblip.cpp

std::vector<RadarBlip*> blips;

RadarBlip::RadarBlip()
{
    blips.push_back(this);
}

RadarBlip::~RadarBlip()
{
    auto it = std::find(blips.begin(), blips.end(), this);

    if (it == blips.end()) { LOGE("Something wrong with delete on RadarBlip"); }

    blips.erase(it);
}

void RadarBlip::Destroy()
{
    delete this;
}

void RadarBlip::DrawAll()
{
    //logger->Info("draw all");

    for (auto blip : blips)
    {
        CVector2D radarPoint;
        TransformRealWorldPointToRadarSpace(radarPoint, blip->worldPosition);

        float distance = LimitRadarPoint(radarPoint);

        float opacity = 1.0f;

        if (blip->dontDrawOnBorder && !gMobileMenu.m_bOpenMenuMap)
        {
            constexpr float DISTANCE_FULL_OPACITY = 0.8f;
            constexpr float DISTANCE_ZERO_OPACITY = 0.95f;

            opacity = 1.0f - std::clamp((distance - DISTANCE_FULL_OPACITY) / (DISTANCE_ZERO_OPACITY - DISTANCE_FULL_OPACITY), 0.0f, 1.0f);
        }

        CRGBA finalColor = blip->color;
        finalColor.a = opacity * 255;

        DrawUtils::DrawTextureOnRadar((Texture*)blip->texture, radarPoint, CVector2D(blip->size, blip->size), finalColor);
    }
}

std::vector<RadarBlip*>& RadarBlip::GetAll()
{
    return blips;
}