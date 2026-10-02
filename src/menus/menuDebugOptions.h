#pragma once

#include "../config.h"
#include "../utils/quickConfig.h"
#include "../window/windowManager.h"

#include "../audio/audioUtils.h"
#include "../radarBlip//radarBlip.h"
#include "colorPickerMenu.h"
#include "introductionImage.h"

inline QuickConfig* debugOptions = nullptr;

#define STR_hide_screen_info_messages "hide_screen_info_messages"

inline bool hide_screen_info_messages()
{
    if (debugOptions == nullptr) return true;

    return *debugOptions->GetBool(STR_hide_screen_info_messages);
}

static void CreateMenuDebugOptionsQuickConfig()
{
    std::string configPath = GetMenuFolder() + "debugOptions.ini";

    debugOptions = new QuickConfig(configPath);

    debugOptions->AddBool(STR_hide_screen_info_messages, true);

    debugOptions->AddBool("draw_swipe_areas", false);
    debugOptions->AddBool("draw_container_boundings", false);

    ScreenDebug::Main->hideInfoMessages = *debugOptions->GetBool(STR_hide_screen_info_messages);
}

static void CreateMenuDebugOptions()
{
    static CRGBA g_color = CRGBA(0, 255, 0);
    static float g_float = 0.75f;

    auto window = WindowManager::CreateWindow(500, 500, "Debug Options", "", 800.0f);

    window->AddButton("CreateIntroduction", []() { CreateIntroduction(); });

    {
        auto slider = window->AddSlider("Blip size", &testBlipSize, 1, 500, 0);
        slider->onValueChange->Add(
            []()
            {
                auto blips = RadarBlip::GetAll();

                for (auto blip : blips) { blip->size = testBlipSize; }
            });
    }

    window->AddCheckbox("draw_swipe_areas", debugOptions->GetBool("draw_swipe_areas"));

    window->AddCheckbox("draw_container_boundings", debugOptions->GetBool("draw_container_boundings"));

    {
        auto checkbox = window->AddCheckbox(STR_hide_screen_info_messages, debugOptions->GetBool(STR_hide_screen_info_messages));

        checkbox->onValueChange->Add([checkbox]() { ScreenDebug::Main->hideInfoMessages = checkbox->GetBoolValue(); });
    }

    window->AddButton("~y~Clear logs", []() { ScreenDebug::Main->Clear(); });

    {
        auto item = window->AddButton("Test mp3", []() { PlayTestMp3(); });
        item->AddIcon(GetMenuAssetPath("icons/script.png"));
    }

    {
        auto button = window->AddButton("Choose color",
            [window]()
            {
                window->blocked = true;

                auto colorPickerWindow = CreateColorPickerMenu("Choose color", &g_color);
                colorPickerWindow->onClose->Add([window]() { window->blocked = false; });
            });

        button->AddColorPreview(&g_color);
    }

    window->AddSlider_Internal("Slider", &g_float, 1.0f, 100.0f, 0);

    {
        window->AddButton("~r~(warning) Crash my game",
            []()
            {
                volatile int* ptr = nullptr;
                *ptr = 123;
            });
    }

    window->onClose->Add([]() { debugOptions->Save(); });
}