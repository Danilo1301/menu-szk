#pragma once

#include "../container/container.h"
#include "../menuInterface.h"
#include "mod/logger.h"

static void Ease_Curve(Container *container, CVector2D startPosition, CVector2D endPosition, CVector2D startScale,
    CVector2D endScale, float startOpacity, float endOpacity, int duration, std::function<void()> onComplete = nullptr)
{
    const CVector2D control1(startPosition.x + (endPosition.x - startPosition.x) * 0.25f,

        startPosition.y + (endPosition.y - startPosition.y) * 0.25f + 100.0f);

    const CVector2D control2(startPosition.x + (endPosition.x - startPosition.x) * 0.75f,

        startPosition.y + (endPosition.y - startPosition.y) * 0.75f + 100.0f);

    menuInterface->onPreRenderEnd->AddUntil(
        [=, elapsed = 0.0f](int deltaTime) mutable -> bool
        {
            elapsed += deltaTime;

            float t = elapsed / (float)duration;

            if (t > 1.0f)
                t = 1.0f;

            const float eased = t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;

            const float inverse = 1.0f - eased;

            CVector2D position(inverse * inverse * inverse * startPosition.x +
                    3.0f * inverse * inverse * eased * control1.x + 3.0f * inverse * eased * eased * control2.x +
                    eased * eased * eased * endPosition.x,

                inverse * inverse * inverse * startPosition.y + 3.0f * inverse * inverse * eased * control1.y +
                    3.0f * inverse * eased * eased * control2.y + eased * eased * eased * endPosition.y);

            container->style.left = std::to_string(position.x) + "px";

            container->style.top = std::to_string(position.y) + "px";

            container->style.right = "auto";
            container->style.bottom = "auto";

            container->style.scale = CVector2D(startScale.x + (endScale.x - startScale.x) * eased,

                startScale.y + (endScale.y - startScale.y) * eased);

            container->style.opacity = startOpacity + (endOpacity - startOpacity) * eased;

            if (t >= 1.0f)
            {
                if (onComplete)
                    onComplete();

                return false;
            }

            return true;
        });
}

static void Ease_Simple(Container *container, CVector2D startPosition, CVector2D endPosition, CVector2D startScale,
    CVector2D endScale, float startOpacity, float endOpacity, int duration, std::function<void()> onComplete = nullptr)
{
    logger->Info("adding until");

    menuInterface->onPreRenderEnd->AddUntil(
        [=, elapsed = 0](unsigned int deltaTime) mutable -> bool
        {
            logger->Info("deltaTime = %d", deltaTime);

            elapsed += deltaTime;

            float t = elapsed / (float)duration;

            if (t > 1.0f)
                t = 1.0f;

            float eased = t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;

            CVector2D position(startPosition.x + (endPosition.x - startPosition.x) * eased,

                startPosition.y + (endPosition.y - startPosition.y) * eased);

            container->style.left = std::to_string(position.x) + "px";

            container->style.top = std::to_string(position.y) + "px";

            container->style.right = "auto";
            container->style.bottom = "auto";

            container->style.scale = CVector2D(startScale.x + (endScale.x - startScale.x) * eased,

                startScale.y + (endScale.y - startScale.y) * eased);

            container->style.opacity = startOpacity + (endOpacity - startOpacity) * eased;

            if (t >= 1.0f)
            {
                if (onComplete)
                    onComplete();

                return false;
            }

            return true;
        });
}