#include "menuSZK.h"

#include "aml-psdk/gta_base/RGBA.h"
#include "aml-psdk/gta_base/Vector.h"
#include "bottomPanel/bottomPanel.h"
#include "cellphone/cellphone.h"
#include "config.h"
#include "container/container.h"
#include "global_config.h"
#include "input.h"
#include "keyboard/keyboard.h"
#include "menus/colorPickerMenu.h"
#include "menus/menuDebugOptions.h"
#include "mod/logger.h"
#include "pch.h"
#include "peds.h"
#include "utils/drawUtils.h"
#include "utils/textureLoader.h"

#include "window/m_menu.h"
#include "window/windowManager.h"
#include <functional>
#include <string>
#include <vector>

#include "menuInterface.h"

#include "cellphone/cellphone.h"

unsigned int _customTime = 0;
unsigned int _lastTime = 0;

void MenuSZK::OnInitialize()
{
    logger->Info("Calling LOGI");
    LOGI("Hello");
    logger->Info("ok done");

    Input::Initialize();

    CreateMenuDebugOptionsQuickConfig();

    Container *mainContainer = Container::MainContainer = Container::CreateContainer("main-container");
    // mainContainer->fillHorizontal = true;
    // mainContainer->fillVertical = true;
    // mainContainer->style.drawBackground = false;

    auto bottomPanel = BottomPanel::CreateMain();
    auto cellphone = Cellphone::CreateScriptsCellphone();

    auto windowIcon = GetMenuAssetPath("icons/window.png");

    cellphone->AddItem("Menu Debug Options", windowIcon, []() { CreateMenuDebugOptions(); });

    auto keyboardIcon = GetMenuAssetPath("icons/keyboard.png");

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

    cellphone->AddItem("Criar teste menu", "",
        []()
        {
            auto res = DrawUtils::GetBaseResolution();
            auto pos = CVector2D(res.width / 2.0f, res.height / 2.0f);

            auto window = WindowManager::CreateWindow(pos.x, pos.y, "Test menu", "Subtitle of this menu", 800);

            static bool g_bool = false;
            static int g_int = 2;
            static float g_float = 0.75f;
            static CRGBA g_color = CRGBA(0, 255, 0);

            logger->Info("adding items?");

            for (int i = 0; i < 1; i++)
            {
                window->AddItem("Hello " + std::to_string(i));
            }

            {
                window->AddButton("Button test", []() {
                    
                });
            }

            {
                auto button = window->AddButton("Choose color",
                    [window]()
                    {
                        window->blocked = true;

                        auto colorPickerWindow = CreateColorPickerMenu("Choose color", &g_color);
                        colorPickerWindow->onClose->Add([window]() { window->blocked = false; });
                    });

                button->AddColorPreview(&g_color);
            }

            for (int i = 0; i < 1; i++)
            {
                window->AddSlider_Internal("Slider", &g_float, 1.0f, 100.0f, 0);
            }

            {
                auto item = window->AddCheckbox_Internal("Checkbox", &g_bool);
            }

            {
                auto options = window->AddOptions_Internal("Options", 500.0f);
                options->AddOption(0, "Low");
                options->AddOption(1, "Medium");
                options->AddOption(2, "High");
            }

            // {
            //     auto options = window->AddIntOptions_Internal("int optiosn", &g_int, 0, 20, 1);
            // }

            // {
            //     auto options = window->AddFloatOptions_Internal("float optiosn", &g_float, 0, 100.0f, 0.2f);
            // }

            //
        });

    cellphone->AddItem("Crash game", "",
        []()
        {
            volatile int *ptr = nullptr;
            *ptr = 123;
        });

    auto scriptsIcon = GetMenuAssetPath("icons/scripts.png");
    auto cellphoneIcon = GetMenuAssetPath("icons/cellphone.png");

    bottomPanel->AddItem("Scripts", scriptsIcon, [] {});
    bottomPanel->AddItem("Celular", cellphoneIcon, [] { Cellphone::ScriptsCellphone->FadeIn(); });

    // Keyboard::SetVisible(true);

    // TestCurl();
}

void MenuSZK::OnTimerUpdate()
{
    DrawUtils::TryFindResolution();

    WindowManager::CloseRequestedWindows();
}

void MenuSZK::OnGameProcess()
{
    Peds::Process();

    // for (int i = 0; i < CPools::ms_pPedPool->m_nSize; i++)
    // {
    //     CPed* ped = CPools::ms_pPedPool->GetAt(i);

    //     if (!ped)
    //         continue;

    //     int ref = CPools::ms_pPedPool->GetRef(ped);

    //     //logger->Info("PedPool: index=%d ped=%p ref=%d", i, ped, ref);
    // }
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