#include "menuItemSlider.h"

#include "../../config.h"
#include "menu/menu.h"
#include <cstddef>
#include <functional>

#include "../slider.h"

MenuItemSlider::MenuItemSlider(Window *window, float *pValue, float minValue, float maxValue, int decimals)
    : MenuItem(window)
{
    auto sliderPanel = container->AddChild("sliderPanel");

    auto slider = new Slider(sliderPanel, pValue, minValue, maxValue, decimals);

    sliderPanel->style.right = "0px";
    sliderPanel->style.top = "0px";

    sliderPanel->style.left = "auto";
    sliderPanel->style.bottom = "auto";

    slider->onValueChange = [this]() { onValueChange->Emit(); };
}

MenuItemSlider::~MenuItemSlider() {}