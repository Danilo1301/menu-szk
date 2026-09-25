#include "config.h"
#include "menu/menu.h"
#include "menuInterface.h"
#include "mod/interface.h"
#include "mod/logger.h"
#include "pch.h"

#include "hooks.h"
#include "menuSZK.h"
#include "utils/logStorage.h"
#include "utils/utils.h"
#include "webServer/webServer.h"
#include <string>

MYMODCFG(com.daniloszk.menuszk_v2, Menu SZK, 1.0, DaniloSZK)

ON_GAME_CRASH() { SaveToFile(); }

ON_MOD_PRELOAD()
{
    logger->SetTag("MenuSZK-PSDK");

    logger->Info("Mod preloaded");

    InitLogStorage();

    MenuSZK::OnInitialize();

    logger->Info("Downloading static files...");

    std::string result = DownloadAndGetContent(
        "https://raw.githubusercontent.com/Danilo1301/static-archives/refs/heads/main/MENU_SZK_V2");

    std::string baseUrl = "";

    size_t pos = result.find("url=");
    if (pos != std::string::npos)
    {
        baseUrl = result.substr(pos + 4);

        while (!baseUrl.empty() && (baseUrl.back() == '\n' || baseUrl.back() == '\r' || baseUrl.back() == ' '))
            baseUrl.pop_back();
    }

    if (baseUrl.find("trycloudflare.com") != std::string::npos)
    {
        baseUrl = "https://proxy-test-app.fly.dev";
    }
    if (baseUrl.find("http") != 0)
    {
        baseUrl = "https://" + baseUrl;
    }
    WebServer::BaseURL = baseUrl;

    logger->Info("%s", result.c_str());

    WebServer::Initialze();

    RegisterInterface("menuSZK_v2", (IMenuSZK *)menuInterface);
}

ON_MOD_LOAD()
{
    logger->Info("Mod loaded");

    DoHooks();
}