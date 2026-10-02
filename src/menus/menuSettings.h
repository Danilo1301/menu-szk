#pragma once

#include "../config.h"
#include "../utils/quickConfig.h"
#include "../window/windowManager.h"

#include "../audio/audioUtils.h"
#include "../radarBlip//radarBlip.h"
#include "introductionImage.h"
#include <string>

inline QuickConfig* menuSettings = nullptr;

static void CreateMenuSettingsQuickConfig()
{
    std::string configPath = GetMenuFolder() + "settings.ini";

    menuSettings = new QuickConfig(configPath);

    menuSettings->AddString("language", Localization::currentLanguage);

    Localization::SetLanguage(*menuSettings->GetString("language"));

    menuSettings->Save();
}