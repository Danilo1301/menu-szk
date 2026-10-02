#pragma once

#include "aml-psdk/gta_base/Vector.h"
#include "src/config.h"
#include "src/container/container.h"
#include <regex>
#include <string>

#include "../utils/effects.h"
#include "../input.h"
#include "../utils/downloadControlled.h"
#include "../utils/utils.h"

inline std::vector<std::string> GetIntroImages()
{
    const std::string& folder = GetMenuAssetPath("/intro/");

    std::vector<std::string> result;
    const std::regex pattern(R"(image_\d+\.png)", std::regex::icase);

    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        if (!entry.is_regular_file()) continue;

        const std::string filename = entry.path().filename().string();

        if (std::regex_match(filename, pattern)) result.push_back(entry.path().string());
    }

    return result;
}

inline void CreateIntroduction()
{
    logger->Info("CreateIntroduction");

    static void* myRef = (void*)0x222215;

    if (!Container::MainContainer)
    {
        logger->Info("CreateIntroduction failed: MainContainer is null");
        return;
    }

    const std::string menuPngFile = GetMenuAssetPath("intro/image_menu_credits.png");

    auto container = Container::MainContainer->AddChild("intro");

    if (!container)
    {
        logger->Info("CreateIntroduction failed: could not create intro container");
        return;
    }

    container->style.left = "50%";
    container->style.top = "50%";
    container->style.transformOrigin = CVector2D(0, 0);
    container->style.width = "100%";
    container->style.height = "100%";
    container->style.opacity = 0;
    container->canBlockTouchEvents = true;

    auto AddImage = [container](const std::string& imagePath)
    {
        if (imagePath.empty()) return;

        if (!std::filesystem::exists(imagePath))
        {
            logger->Info("Introduction image does not exist: %s", imagePath.c_str());
            return;
        }

        auto image = container->AddChild("image");

        if (!image)
        {
            logger->Info("Failed to create introduction image container");
            return;
        }

        image->style.width = "100%";
        image->style.height = "100%";
        image->style.backgroundImage = imagePath;
    };

    auto userImages = GetIntroImages();

    for (const std::string& imagePath : userImages) AddImage(imagePath);

    AddImage(menuPngFile);

    struct IntroductionState
    {
        Container* container = nullptr;
        bool canFadeOut = false;
        bool destroyed = false;
    };

    auto state = std::make_shared<IntroductionState>();
    state->container = container;

    SetTimeout(
        [state]()
        {
            if (state->destroyed || !state->container) return;

            state->canFadeOut = true;
        },
        350);

    auto destroy = [state]()
    {
        if (state->destroyed) return;

        state->destroyed = true;

        Input::OnSwipeSpecial->Remove(myRef);

        Container* container = state->container;

        if (!container) return;

        state->container = nullptr;

        container->Destroy();
    };

    auto fadeOut = [state, destroy]()
    {
        if (state->destroyed || !state->container) return;

        Container* container = state->container;

        if (container->onClick) container->onClick->Clear();

        container->canBlockTouchEvents = false;

        Ease_ScaleOpacity(container,
            CVector2D(1, 1),
            CVector2D(1, 1),
            1,
            0,
            500,
            [state, destroy]()
            {
                if (state->destroyed) return;

                destroy();
            });
    };

    auto fadeIn = [state]()
    {
        if (state->destroyed || !state->container) return;

        Ease_ScaleOpacity(state->container, CVector2D(1.2f, 1.2f), CVector2D(1, 1), 0, 1, 1000, []() {});
    };

    fadeIn();

    Input::OnSwipeSpecial->AddRef(myRef,
        [state, fadeOut](int id, std::string specialId)
        {
            if (state->destroyed || !state->container) return;

            if (specialId != "bottom_panel_swipe_up") return;

            if (!state->canFadeOut) return;

            state->canFadeOut = false;

            fadeOut();
        });

    logger->Info("CreateIntroduction passed");
}

inline void DownloadIntroductionImage()
{
    std::thread(
        []()
        {
            auto menuPngFile = GetMenuAssetPath("intro/image_menu_credits.png");

            DownloadIfPossible("https://raw.githubusercontent.com/Danilo1301/static-archives/main/MENUSZK_CREDITS.png", menuPngFile);
        })
        .detach();
}