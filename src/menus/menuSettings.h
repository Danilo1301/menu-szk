#pragma once

#include "../config.h"
#include "../utils/quickConfig.h"
#include "../window/windowManager.h"

#include "../audio/audioUtils.h"
#include "../radarBlip//radarBlip.h"
#include "introductionImage.h"
#include "mod/logger.h"
#include "src/utils/utils.h"
#include <string>

inline QuickConfig* menuSettings = nullptr;

inline void CreateMenuSettingsQuickConfig()
{
    logger->Info("1");

    std::string configPath = GetMenuFolder() + "settings.ini";

    menuSettings = new QuickConfig(configPath);

    //

    menuSettings->AddString("language", Localization::currentLanguage);

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

    logger->Info("2");
}

inline bool use_old_input_system()
{
    return *menuSettings->GetBool("use_simple_input_system");
}