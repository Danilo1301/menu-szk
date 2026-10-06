#pragma once

#include "../config.h"
#include "../utils/quickConfig.h"
#include "../localization/localization.h"

#include "../utils/utils.h"
#include <string>

inline QuickConfig* menuSettings = nullptr;

inline void CreateMenuSettingsQuickConfig()
{
    std::string configPath = GetMenuFolder() + "settings.ini";

    menuSettings = new QuickConfig(configPath);

    //

    menuSettings->AddString("language", Localization::currentLanguage);

    menuSettings->AddString("top_screen_credits_message", "Credits: DaniloSZK (change me!)");

    menuSettings->AddBool("top_screen_credits_message_enabled", true);

    menuSettings->AddBool("use_simple_input_system", false);

    auto inputFilePath = GetMenuAssetPath("TOUCH_TEST");
    if (FileExists(inputFilePath))
    {
        *menuSettings->GetBool("use_simple_input_system") = true;

        RemoveFile(inputFilePath);
    }

    //

    Localization::SetLanguage(*menuSettings->GetString("language"));

    menuSettings->Save();
}

inline bool use_old_input_system()
{
    return *menuSettings->GetBool("use_simple_input_system");
}