#pragma once

#include "../config.h"
#include "../utils/downloadControlled.h"
#include "../menuSZK.h"
#include "aml-psdk/gta_base/Vector.h"
#include "src/utils/drawUtils.h"

#include <fstream>
#include <thread>

inline void BeginNews()
{
    auto newsFile = GetMenuAssetPath("downloaded/news.txt");

    std::thread(
        [newsFile]()
        {
            bool downloaded = DownloadIfPossible("https://raw.githubusercontent.com/Danilo1301/static-archives/main/NEWS.txt", newsFile);

            if (!downloaded) { return; }

            std::ifstream file(newsFile);

            if (!file.is_open()) { return; }

            auto res = DrawUtils::GetBaseResolution();
            auto pos = CVector2D(res.width / 2.0f, res.height / 2.0f);

            auto window = menuSZK->CreateWindow(pos.x, pos.y, 800, "News", "Some news");

            std::string line;

            while (std::getline(file, line))
            {
                if (line.empty()) { continue; }

                window->AddItem(line);
            }
        })
        .detach();
}