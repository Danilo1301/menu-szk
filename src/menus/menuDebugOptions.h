#pragma once

#include "../config.h"
#include "../utils/quickConfig.h"
#include "../window/windowManager.h"

inline QuickConfig *debugOptions = nullptr;

static void CreateMenuDebugOptionsQuickConfig()
{
    std::string configPath = GetMenuFolder() + "debugOptions.ini";

    debugOptions = new QuickConfig(configPath);

    debugOptions->AddBool("draw_swipe_areas", false);
    debugOptions->AddBool("draw_debug_on_screen", false);
    debugOptions->AddBool("draw_container_boundings", false);

    ScreenDebug::Main->onlyLogErrors = !*debugOptions->GetBool("draw_debug_on_screen");
}

static void CreateMenuDebugOptions()
{
    auto window = WindowManager::CreateWindow(500, 500, "Debug Options", "", 800.0f);

    window->AddCheckbox("draw_swipe_areas", debugOptions->GetBool("draw_swipe_areas"));

    window->AddCheckbox("draw_container_boundings", debugOptions->GetBool("draw_container_boundings"));

    {
        auto checkbox = window->AddCheckbox("draw_debug_on_screen", debugOptions->GetBool("draw_debug_on_screen"));
        checkbox->onValueChange->Add([checkbox]() { ScreenDebug::Main->onlyLogErrors = !checkbox->GetBoolValue(); });
    }

    window->AddButton("Clear logs", []() { ScreenDebug::Main->Clear(); });

    window->onClose->Add([]() { debugOptions->Save(); });
}