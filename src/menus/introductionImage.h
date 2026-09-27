#pragma once

#include "aml-psdk/gta_base/Vector.h"
#include "src/config.h"
#include "src/container/container.h"
#include <string>

#include "../utils/effects.h"
#include "src/utils/utils.h"

inline void CreateIntroductionImage(const std::string &pngPath)
{
    logger->Info("CreateIntroductionImage");

    auto container = Container::MainContainer->AddChild("intro");

    container->style.left = "50%";
    container->style.top = "50%";
    container->style.transformOrigin = CVector2D(0, 0);
    container->style.width = "100%";
    container->style.height = "100%";
    container->style.opacity = 0;
    container->style.backgroundImage = pngPath;

    struct IntroductionState
    {
        Container *container = nullptr;
        bool canFadeOut = false;
        bool destroyed = false;
    };

    auto state = std::make_shared<IntroductionState>();
    state->container = container;

    SetTimeout(
        [state]()
        {
            if (state->destroyed)
                return;

            state->canFadeOut = true;
        },
        2000);

    auto destroy = [state]()
    {
        if (state->destroyed)
            return;

        state->destroyed = true;

        Container *container = state->container;

        if (!container)
            return;

        state->container = nullptr;

        container->Destroy();
    };

    auto fadeOut = [state, destroy]()
    {
        if (state->destroyed || !state->container)
            return;

        Container *container = state->container;

        Ease_ScaleOpacity(container, CVector2D(1, 1), CVector2D(1, 1), 1, 0, 1000,
            [state, destroy]()
            {
                if (state->destroyed)
                    return;

                destroy();
            });
    };

    auto fadeIn = [state]()
    {
        if (state->destroyed || !state->container)
            return;

        Ease_ScaleOpacity(state->container, CVector2D(1.2f, 1.2f), CVector2D(1, 1), 0, 1, 1000, []() {});
    };

    container->onClick->Add(
        [state, fadeOut]()
        {
            if (state->destroyed || !state->container)
                return;

            if (!state->canFadeOut)
                return;

            state->canFadeOut = false;

            fadeOut();
        });

    fadeIn();
}

inline void CreateIntroduction()
{
    logger->Info("CreateIntroduction");

    auto pngFile = GetMenuAssetPath("intro/image1.png");

    CreateIntroductionImage(pngFile);
}