#include "pch.h"

#include "audio/soundSystem/CSoundSystem.h"
#include "hooks.h"
#include "logHelper.h"
#include "menu/menu.h"
#include "menuInterface.h"
#include "menuSZK.h"
#include "mod/interface.h"
#include "mod/logger.h"
#include "utils/logStorage.h"
#include <string>

MYMODCFG(com.daniloszk.menuszk_v2, Menu SZK, 1.0, DaniloSZK)

ON_GAME_CRASH() { LogHelper::CreateLogFile(); }

ON_MOD_PRELOAD()
{
    logger->SetTag("MenuSZK-PSDK");

    logger->Info("Mod preloading...");

    LogHelper::Initialize("menuSZK");
    LogHelper::SetFrameOperation("ON_MOD_PRELOAD");

    MenuSZK::OnPreload();

    logger->Info("Registering menuSZK_v2 interface...");

    RegisterInterface("menuSZK_v2", (IMenuSZK *)menuInterface);

    logger->Info("Mod preloaded");
}

ON_MOD_LOAD()
{
    LogHelper::SetFrameOperation("ON_MOD_LOAD");

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