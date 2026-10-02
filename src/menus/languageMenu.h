#pragma once

#include "../localization/localization.h"
#include "../menuSZK.h"
#include "aml-psdk/gta_base/Vector.h"
#include "menuSettings.h"
#include "src/config.h"
#include "src/utils/drawUtils.h"

inline void CreateChangeLanguageMenu()
{
    auto screenResolution = DrawUtils::GetBaseResolution();
    auto menuPos = CVector2D(screenResolution.width / 2.0f, screenResolution.height / 2.0f);

    auto window = menuSZK->CreateWindow(menuPos.x, menuPos.y, 800, "Language", "Select a language");
    window->AddItem("Choose a language below:");

    auto languages = Localization::GetLanguages();

    for (auto language : languages)
    {
        auto button = window->AddButton(language,
            [window, language]()
            {
                window->Close();

                Localization::SetLanguage(language);

                *menuSettings->GetString("language") = language;
                menuSettings->Save();
            });

        auto pngFile = GetMenuAssetPath("flags/" + language + ".png");

        button->AddIcon(pngFile);
    }

    {
        auto button = window->AddButton("~r~Close", [window]() { window->Close(); });
    }
}