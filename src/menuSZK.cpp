#include "menuSZK.h"

#include "aml-psdk/game_sa/utils/OpcodeCaller_fixed.h"
#include "bottomPanel/bottomPanel.h"
#include "cellphone/cellphone.h"
#include "config.h"
#include "container/container.h"
#include "globals.h"
#include "input.h"
#include "keyboard/keyboard.h"
#include "menu/menu.h"
#include "menuInterface.h"
#include "menus/introductionImage.h"
#include "menus/menuDebugOptions.h"
#include "mod/logger.h"
#include "peds.h"
#include "src/menuInterface.h"
#include "src/screenDebug/screenDebug.h"
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

void MenuSZK::OnPreload()
{
    auto windowIcon = GetMenuAssetPath("icons/window.png");
    auto keyboardIcon = GetMenuAssetPath("icons/keyboard.png");
    auto scriptsIcon = GetMenuAssetPath("icons/scripts.png");
    auto cellphoneIcon = GetMenuAssetPath("icons/cellphone.png");

    //

    Peds::onPedAdded->Add([](GameEntity e) { menuInterface->onPedAdded->Emit(e); });
    Peds::onPedRemoved->Add([](GameEntity e) { menuInterface->onPedRemoved->Emit(e); });

    Vehicles::onVehicleAdded->Add([](GameEntity e) { menuInterface->onVehicleAdded->Emit(e); });
    Vehicles::onVehicleRemoved->Add([](GameEntity e) { menuInterface->onVehicleRemoved->Emit(e); });

    //

    ScreenDebug::Main->AddLine("MenuSZK initialized. Author: DaniloSZK", ScreenLogType::Special);

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

    cellphone->AddItem("Show keyboard", keyboardIcon,
        []()
        {
            if (!Keyboard::IsVisible())
            {
                Keyboard::SetVisible(true);
            }
            else
            {
                Keyboard::SetVisible(false);
            }
        });

    cellphone->AddItem("Criar teste menu", "", []() { CreateTestMenu(); });

    //

    logger->Info("Creating bottomPanel items...");

    bottomPanel->AddItem("Scripts", scriptsIcon, [] {});
    bottomPanel->AddItem("Celular", cellphoneIcon, [] { Cellphone::ScriptsCellphone->FadeIn(); });

    //

    Input::Initialize();

    logger->Info("CreateMenuDebugOptionsQuickConfig");

    CreateMenuDebugOptionsQuickConfig();
}

void MenuSZK::OnLoad()
{
    SetupOnPlayerReady();

    // Keyboard::SetVisible(true);
    // TestCurl();

    // auto widget = menuInterface->CreateWidget(400, 100, 200, "", image);
    // widget->onClick->Add(
    //     []()
    //     {
    //         auto info = InfoMessage::GetBottom();

    //         info->SetMessage("Clicked widget", 1000);

    //         PlayTestMp3();
    //     });

    WebServer::Initialze();

    if (!WebServer::Joined)
    {
        LOGW("Failed to connect to server");
    }
}

void MenuSZK::OnTimerUpdate()
{
    DrawUtils::TryFindResolution();

    WindowManager::CloseRequestedWindows();
    Container::DestroyContainersThatNeedsToBeDestroyed();
}

void MenuSZK::OnGameProcess()
{
    Peds::Process();
    Vehicles::Process();
}

void MenuSZK::OnRender()
{
    ProcessTexturesCallbacks();

    if (Container::MainContainer)
    {
        LOG_PER_FRAME("updating main container");

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

    LOG_PER_FRAME("MenuSZK::OnRender [end]");
}

void MenuSZK::SetupOnPlayerReady()
{
    menuInterface->onScriptProcess->AddUntil(
        [](unsigned int dt)
        {
            if (!Command<Commands::IS_PLAYER_PLAYING>(0))
                return true;

            int playerActor;
            Command<Commands::GET_PLAYER_CHAR>(0, &playerActor);

            if (playerActor == -1)
                return true;

            logger->Info("MenuSZK: Emitting onPlayerReady");

            menuInterface->onPlayerReady->Emit();

            SetTimeout([]() { CreateIntroduction(); }, 3000);

            return false;
        });
}