#pragma once

#include "../menus/colorPickerMenu.h"
#include "../utils/drawUtils.h"
#include "../window/windowManager.h"


inline void CreateTestMenu()
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
        window->AddCustomItem("~y~Custom item", 400);
    }

    {
        auto options = window->AddOptions_Internal("Options", 500.0f);
        options->AddOption(0, "Low");
        options->AddOption(1, "Medium");
        options->AddOption(2, "High");
    }

    {
        window->AddButton("~r~Crash game ~y~[caution]",
            []()
            {
                volatile int *ptr = nullptr;
                *ptr = 123;
            });
    }

    // {
    //     auto options = window->AddIntOptions_Internal("int optiosn", &g_int, 0, 20, 1);
    // }

    // {
    //     auto options = window->AddFloatOptions_Internal("float optiosn", &g_float, 0, 100.0f, 0.2f);
    // }

    //
}