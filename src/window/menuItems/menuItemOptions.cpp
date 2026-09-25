#include "menuItemOptions.h"

#include "../../config.h"
#include "menu/menu.h"
#include <cstddef>
#include <functional>

#include "../../container/containerLoader.h"

MenuItemOptions::MenuItemOptions(Window *window, float optionsWidth) : MenuItem(window)
{
    containers = CreateOptionsContainers(container, optionsWidth);

    containers.left->onClick->Add(
        [this]()
        {
            if (type == OptionsType::List)
            {
                if (options.empty())
                    return;

                if (optionsIndex > 0)
                    optionsIndex--;
            }
            else if (type == OptionsType::Integer)
            {
                *integerValue -= integerStep;

                if (*integerValue < integerMin)
                    *integerValue = integerMin;
            }
            else if (type == OptionsType::Float)
            {
                *floatValue -= floatStep;

                if (*floatValue < floatMin)
                    *floatValue = floatMin;
            }
            else
            {
                return;
            }

            UpdateOptionsValue();

            InvokeOnChangedValue();
        });

    containers.right->onClick->Add(
        [this]()
        {
            if (type == OptionsType::List)
            {
                if (options.empty())
                    return;

                if (optionsIndex + 1 < (int)options.size())
                    optionsIndex++;
            }
            else if (type == OptionsType::Integer)
            {
                *integerValue += integerStep;

                if (*integerValue > integerMax)
                    *integerValue = integerMax;
            }
            else if (type == OptionsType::Float)
            {
                *floatValue += floatStep;

                if (*floatValue > floatMax)
                    *floatValue = floatMax;
            }
            else
            {
                return;
            }

            UpdateOptionsValue();

            InvokeOnChangedValue();
        });
}

MenuItemOptions::~MenuItemOptions() {}

void MenuItemOptions::AddOption_Internal(int value, std::string text)
{
    type = OptionsType::List;

    options.push_back({value, text});

    UpdateOptionsValue();
}

void MenuItemOptions::SetInteger(int *value, int min, int max, int step)
{
    type = OptionsType::Integer;

    integerValue = value;
    integerMin = min;
    integerMax = max;
    integerStep = step;

    UpdateOptionsValue();
}

void MenuItemOptions::SetFloat(float *value, float min, float max, float step)
{
    type = OptionsType::Float;

    floatValue = value;
    floatMin = min;
    floatMax = max;
    floatStep = step;

    UpdateOptionsValue();
}

void MenuItemOptions::UpdateVisuals(bool leftState, bool rightState)
{
    auto leftPng = leftState ? GetMenuAssetPath("menu/button_left.png") : GetMenuAssetPath("menu/button_left_off.png");
    auto rightPng =
        rightState ? GetMenuAssetPath("menu/button_right.png") : GetMenuAssetPath("menu/button_right_off.png");

    containers.left->style.backgroundImage = leftPng;
    containers.right->style.backgroundImage = rightPng;

    // if (leftTexture)
    // {
    //     containers.left->SetBackgroundTexture(leftTexture);
    // }

    // if (rightTexture)
    // {
    //     containers.right->SetBackgroundTexture(rightTexture);
    // }
}

void MenuItemOptions::UpdateOptionsValue()
{
    bool leftState = false;
    bool rightState = false;

    if (type == OptionsType::List)
    {
        if (options.empty())
        {
            containers.value->text = "";
        }
        else
        {
            containers.value->text = options[optionsIndex].second;

            leftState = optionsIndex > 0;
            rightState = optionsIndex + 1 < (int)options.size();
        }
    }
    else if (type == OptionsType::Integer)
    {
        containers.value->text = std::to_string(*integerValue);

        leftState = *integerValue > integerMin;
        rightState = *integerValue < integerMax;
    }
    else if (type == OptionsType::Float)
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.1f", *floatValue);

        containers.value->text = buffer;

        leftState = *floatValue > floatMin;
        rightState = *floatValue < floatMax;
    }
    else
    {
        containers.value->text = "";
    }

    UpdateVisuals(leftState, rightState);
}

void MenuItemOptions::InvokeOnChangedValue() { onValueChange->Emit(); }

OptionsContainers MenuItemOptions::CreateOptionsContainers(Container *root, float optionsWidth)
{
    OptionsContainers containers;

    auto options = root->AddChild("options");
    LoadContainerFromFile(options, GetMenuLayoutPath("options.json"));

    containers.left = options->FindChild("buttonLeft");
    containers.value = options->FindChild("value");
    containers.right = options->FindChild("buttonRight");

    return containers;
}