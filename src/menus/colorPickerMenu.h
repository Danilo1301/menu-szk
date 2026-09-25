#pragma once

#include "aml-psdk/gta_base/RGBA.h"

#include "../config.h"
#include "../window/windowManager.h"
#include "menu/menu.h"

static IWindow *CreateColorPickerMenu(std::string subtitle, CRGBA *color)
{
    auto window = WindowManager::CreateWindow(500, 500, "Color Picker", subtitle, 700.0f);

    float *r_value = new float(color->r);
    float *g_value = new float(color->g);
    float *b_value = new float(color->b);
    float *a_value = new float(color->a);

    auto onValueChanged = [color, r_value, g_value, b_value, a_value]()
    {
        color->r = (unsigned char)*r_value;
        color->g = (unsigned char)*g_value;
        color->b = (unsigned char)*b_value;
        color->a = (unsigned char)*a_value;
    };

    {
        auto preview = window->AddItem("Cor (preview):");

        preview->AddColorPreview(color);
    }

    {
        auto slider = window->AddSlider_Internal("R ~r~(red)", r_value, 0.0f, 255.0f, 0);

        slider->onValueChange->Add(onValueChanged);
    }

    {
        auto slider = window->AddSlider_Internal("G ~g~(green)", g_value, 0.0f, 255.0f, 0);

        slider->onValueChange->Add(onValueChanged);
    }

    {
        auto slider = window->AddSlider_Internal("B ~b~(blue)", b_value, 0.0f, 255.0f, 0);

        slider->onValueChange->Add(onValueChanged);
    }

    {
        auto slider = window->AddSlider_Internal("A (alpha)", a_value, 0.0f, 255.0f, 0);

        slider->onValueChange->Add(onValueChanged);
    }

    window->onClose->Add(
        [r_value, g_value, b_value, a_value]()
        {
            delete r_value;
            delete g_value;
            delete b_value;
            delete a_value;
        });

    return window;
}