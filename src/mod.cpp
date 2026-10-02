#include "mod.h"

#include "aml-psdk/game_sa/utils/OpcodeCaller_fixed.h"
#include "aml-psdk/gta_base/Vector.h"
#include "bottomPanel/bottomPanel.h"
#include "cellphone/cellphone.h"
#include "config.h"
#include "container/container.h"
#include "globals.h"
#include "infoMessage/infoMessage.h"
#include "input.h"
#include "keyboard/keyboard.h"
#include "localization/localization.h"
#include "menuSZK/imenuSZK.h"
#include "menuSZK.h"
#include "menus/introductionImage.h"
#include "menus/languageMenu.h"
#include "menus/menuDebugOptions.h"
#include "menus/menuSettings.h"
#include "mod/logger.h"
#include "news/news.h"
#include "pch.h"
#include "peds.h"
#include "radarBlip/radarBlip.h"
#include "src/screenDebug/screenDebug.h"
#include "utils/downloadControlled.h"
#include "utils/drawUtils.h"
#include "utils/textureLoader.h"
#include "utils/utils.h"
#include "vehicles.h"
#include "webServer/webServer.h"
#include "window/m_menu.h"
#include "window/testMenu.h"
#include "window/windowManager.h"
#include <functional>
#include <string>

unsigned int _customTime = 0;
unsigned int _lastTime = 0;

void Mod::OnPreload()
{
    {
        std::string dataRootPath = aml->GetAndroidDataRootPath();
        std::string locFolder = dataRootPath + "/mods/data/localization/";

        Localization::RegisterLocalizationRecursively(locFolder);
    }

    //

    auto windowIcon = GetMenuAssetPath("icons/window.png");
    auto keyboardIcon = GetMenuAssetPath("icons/keyboard.png");
    auto scriptsIcon = GetMenuAssetPath("icons/scripts.png");
    auto cellphoneIcon = GetMenuAssetPath("icons/cellphone.png");
    auto languageIcon = GetMenuAssetPath("icons/language.png");

    //

    Peds::Initialize();
    Peds::onPedAdded->Add([](GameEntity e) { menuSZK->onPedAdded->Emit(e); });
    Peds::onPedRemoved->Add([](GameEntity e) { menuSZK->onPedRemoved->Emit(e); });

    Vehicles::Initialize();
    Vehicles::onVehicleAdded->Add([](GameEntity e) { menuSZK->onVehicleAdded->Emit(e); });
    Vehicles::onVehicleRemoved->Add([](GameEntity e) { menuSZK->onVehicleRemoved->Emit(e); });

    //

    ScreenDebug::Main->AddLine("MenuSZK initialized. Author: DaniloSZK", ScreenLogType::Special, VERY_LONG_TIME_MS);

    //

    Container::MainContainer = Container::CreateContainer("main-container");

    //

    logger->Info("Creating bottom panel and cellphone...");

    auto bottomPanel = BottomPanel::CreateMain();
    auto cellphone = Cellphone::CreateScriptsCellphone();

    cellphone->SetVisible(false);

    //

    logger->Info("Creating cellphone items...");

    cellphone->AddItem("Menu Debug Options", windowIcon, []() { CreateMenuDebugOptions(); });

    cellphone->AddItem("Show keyboard",
        keyboardIcon,
        []()
        {
            if (!Keyboard::IsVisible()) { Keyboard::Open(""); }
            else
            {
                Keyboard::Close();
            }
        });

    //

    logger->Info("Creating bottomPanel items...");

    bottomPanel->AddItem("Language", languageIcon, [] { CreateChangeLanguageMenu(); });
    bottomPanel->AddItem("Cellphone", cellphoneIcon, [] { Cellphone::ScriptsCellphone->FadeIn(); });
    bottomPanel->AddItem("Scripts", scriptsIcon, [] {});

    //

    Input::Initialize();

    logger->Info("CreateMenuDebugOptionsQuickConfig");

    CreateMenuSettingsQuickConfig();
    CreateMenuDebugOptionsQuickConfig();

    //

    DownloadIntroductionImage();

    //

    BeginNews();
}

void Mod::OnLoad()
{
    SetupOnPlayerReady();

    // Keyboard::SetVisible(true);
    // TestCurl();

    auto cellphoneIcon = GetMenuAssetPath("icons/cellphone.png");

    // auto widget = menuInterface->CreateWidget(100, 400, 200, "", cellphoneIcon);
    // widget->onClick->Add(
    //     []()
    //     {
    //         menuInterface->onScriptProcess->AddOnce(
    //             [](unsigned int)
    //             {
    //                 int playerActor;
    //                 Command<Commands::GET_PLAYER_CHAR>(0, &playerActor);

    //                 if (playerActor == -1)
    //                 {
    //                     ShowBottomMessage("~r~Invalid player actor?", 2000);
    //                     return;
    //                 }

    //                 ShowBottomMessage("Blip criado!", 2000);

    //                 float x, y, z;
    //                 Command<Commands::GET_CHAR_COORDINATES>(playerActor, &x, &y, &z);

    //                 auto cellphoneIcon = GetMenuAssetPath("icons/cellphone.png");

    //                 RadarBlip* blip = new RadarBlip();
    //                 blip->texture = menuInterface->GetOrLoadTexture(cellphoneIcon);
    //                 blip->worldPosition = CVector(x, y, z);
    //             });
    //     });

    WebServer::Initialze();

    if (!WebServer::Joined) { LOGW("Failed to connect to server"); }
}

void Mod::OnTimerUpdate()
{
    DrawUtils::TryFindResolution();

    WindowManager::CloseRequestedWindows();

    Container::SortContainersThatNeedsToBeSorted();
    Container::DestroyContainersThatNeedsToBeDestroyed();
}

void Mod::OnModProcess()
{
    Peds::Process();
    Vehicles::Process();
}

void Mod::OnRender()
{
    if (g_framesDrawn > 0) ProcessTexturesCallbacks();

    if (Container::MainContainer)
    {
        LOG_PER_FRAME("updating main container");

        Container::MainContainer->Update();
        Container::MainContainer->UpdateTransform();

        LOG_PER_FRAME("drawing main container");

        Container::MainContainer->Draw();

        LOG_PER_FRAME("ended drawing main container");
    }

    RenderExampleMenu();

    // draw swipe

    Input::DrawSwipeAreas();

    //

    ScreenDebug::Main->Draw();

    LOG_PER_FRAME("Mod::OnRender [end]");
}

void Mod::SetupOnPlayerReady()
{
    menuSZK->onScriptProcess->AddUntil(
        [](unsigned int dt)
        {
            if (!Command<Commands::IS_PLAYER_PLAYING>(0)) return true;

            int playerActor;
            Command<Commands::GET_PLAYER_CHAR>(0, &playerActor);

            if (playerActor == -1) return true;

            logger->Info("MenuSZK: Emitting onPlayerReady");

            menuSZK->onPlayerReady->Emit();

            SetTimeout([]() { CreateIntroduction(); }, 3000);

            return false;
        });
}