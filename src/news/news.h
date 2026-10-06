#pragma once

#include "../config.h"
#include "../utils/downloadControlled.h"
#include "../menuSZK.h"
#include "aml-psdk/gta_base/Vector.h"
#include "menuSZK/imenuSZK.h"
#include "src/screenDebug/screenDebug.h"
#include "src/utils/drawUtils.h"

#include <fstream>

inline void BeginNews()
{
    auto newsFile = GetMenuAssetPath("downloaded/news.txt");

    std::ifstream file(newsFile);

    if (!file.is_open()) { return; }

    if (!ScreenDebug::Main) { return; }

    auto res = DrawUtils::GetBaseResolution();
    auto pos = CVector2D(res.width / 2.0f, res.height / 2.0f);

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty()) { continue; }

        ScreenDebug::Main->AddLine(line, ScreenLogType::Special, 10000);
    }
}