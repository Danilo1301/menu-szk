#include "audio/audio.h"
#include "pch.h"

#include "config.h"
#include "menu/menu.h"
#include "menuInterface.h"
#include "mod/interface.h"
#include "mod/logger.h"

#include "audio/soundSystem/CSoundSystem.h"
#include "hooks.h"
#include "logHelper.h"
#include "menuSZK.h"
#include "utils/logStorage.h"
#include "utils/utils.h"
#include "webServer/webServer.h"
#include <string>

MYMODCFG(com.daniloszk.menuszk_v2, Menu SZK, 1.0, DaniloSZK)

ON_GAME_CRASH() { LogHelper::CreateLogFile(); }

ON_MOD_PRELOAD()
{
    logger->SetTag("MenuSZK-PSDK");

    logger->Info("Mod preloading...");

    LogHelper::Initialize("menuSZK");

    MenuSZK::OnPreload();

    logger->Info("Registering menuSZK_v2 interface...");

    RegisterInterface("menuSZK_v2", (IMenuSZK *)menuInterface);

    logger->Info("Mod preloaded");
}

ON_MOD_LOAD()
{
    logger->Info("Mod loaded");

    BASS = (IBASS *)GetInterface("BASS");

    if (!BASS)
    {
        LOGW("libBASSMod.so was not found");
        // logger->Error(" was not found!");
    }

    DoHooks();

    MenuSZK::OnLoad();
}