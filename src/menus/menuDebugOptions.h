#pragma once

#include "../config.h"
#include "../utils/quickConfig.h"
#include "../window/windowManager.h"

#include "../audio/audioUtils.h"
#include "introductionImage.h"

inline QuickConfig *debugOptions = nullptr;

#define STR_hide_screen_info_messages "hide_screen_info_messages"

inline bool hide_screen_info_messages()
{
    if (debugOptions == nullptr)
        return false;

    return *debugOptions->GetBool(STR_hide_screen_info_messages);
}

static void CreateMenuDebugOptionsQuickConfig()
{
    std::string configPath = GetMenuFolder() + "debugOptions.ini";

    debugOptions = new QuickConfig(configPath);

    debugOptions->AddBool(STR_hide_screen_info_messages, false);

    debugOptions->AddBool("draw_swipe_areas", false);
    debugOptions->AddBool("draw_container_boundings", false);

    ScreenDebug::Main->hideInfoMessages = !*debugOptions->GetBool(STR_hide_screen_info_messages);
}

static void CreateMenuDebugOptions()
{
    auto window = WindowManager::CreateWindow(500, 500, "Debug Options", "", 800.0f);

    window->AddCheckbox("draw_swipe_areas", debugOptions->GetBool("draw_swipe_areas"));

    window->AddCheckbox("draw_container_boundings", debugOptions->GetBool("draw_container_boundings"));

    {
        auto checkbox =
            window->AddCheckbox(STR_hide_screen_info_messages, debugOptions->GetBool(STR_hide_screen_info_messages));

        checkbox->onValueChange->Add([checkbox]() { ScreenDebug::Main->hideInfoMessages = !checkbox->GetBoolValue(); });
    }

    window->AddButton("~y~Clear logs", []() { ScreenDebug::Main->Clear(); });

    {
        auto item = window->AddButton("Test mp3", []() { PlayTestMp3(); });
        item->AddIcon(GetMenuAssetPath("icons/script.png"));
    }

    window->AddButton("CreateIntroduction", []() { CreateIntroduction(); });

    window->onClose->Add([]() { debugOptions->Save(); });
}