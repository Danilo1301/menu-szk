#include "pch.h"

#include "menuOperation.h"
#include "audio/soundSystem/CSoundSystem.h"
#include "hooks.h"
#include "logHelper.h"
#include "menuSZK/imenuSZK.h"
#include "menuSZK.h"
#include "mod.h"
#include "mod/interface.h"
#include "mod/logger.h"
#include "utils/logStorage.h"
#include "utils/renameAmlCrashlog.h"
#include <cstdlib>
#include <string>

MYMODCFG(com.daniloszk.menuszk_v2, Menu SZK, 2.1.0, DaniloSZK)

ON_GAME_CRASH()
{
    logger->Info("OnGameCrash()");

    LogHelper::CreateLogFile();
    LogHelper::RemoveOldCrashLogs();

    logger->Info("Log file created!");
}

ON_MOD_PRELOAD()
{
    logger->SetTag("MenuSZK-PSDK");

    LogHelper::Initialize("menuSZK");

    BEGIN_OPERATION(op_ModPreload);

    logger->Info("Mod preloading...");

    RenameAmlLogIfExists();

    Mod::OnPreload();

    logger->Info("Registering menuSZK_v2 interface...");

    RegisterInterface("menuSZK_v2", (IMenuSZK*)menuSZK);

    logger->Info("Mod preloaded");

    END_OPERATION(op_ModPreload);
}

ON_MOD_LOAD()
{
    BEGIN_OPERATION(op_ModLoad);

    logger->Info("Mod loading...");

    BEGIN_OPERATION(op_ResolveDependecies);

    BASS = (IBASS*)GetInterface("BASS");

    if (!BASS) { LOGW("libBASSMod.so was not found"); }

    END_OPERATION_RESULT(op_ResolveDependecies, "we resolved all");

    DoHooks();

    Mod::OnLoad();

    END_OPERATION(op_ModLoad);
}