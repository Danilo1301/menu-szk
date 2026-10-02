#pragma once

#include "../container/container.h"
#include "../pch.h"
#include "menuSZK/imenuSZK.h"
#include "menuItem.h"
#include "menuItems/menuItemCheckbox.h"
#include "menuItems/menuItemOptions.h"
#include "menuItems/menuItemSlider.h"
#include <string>
#include <vector>

class Window : public IWindow
{
private:
    Container* _container;

    Container* _content;
    Container* _title;
    Container* _subTitle;

    Container* _leftButton;
    Container* _rightButton;

    Container* _closeButton;

    std::vector<MenuItem*> items;

    int currentPage = 0;

public:
    Window();
    ~Window();

    bool isClosing = false;

    void Init(float x, float y);

    template <typename T> T* AddItem_T(T* item);

    MenuItem* AddItemDefault();
    MenuItemCheckbox* AddCheckbox_Internal(std::string text, bool* ptr);
    MenuItemOptions* AddOptions_Internal(std::string text, float optionsWidth = 450.0f);
    MenuItemOptions* AddIntOptions_Internal(std::string text, int* ptr, int min, int max, int step);
    MenuItemOptions* AddFloatOptions_Internal(std::string text, float* ptr, float min, float max, float step);
    MenuItem* AddItem_Internal(std::string text);
    MenuItem* AddButton_Internal(std::string text, std::function<void()> onClick);
    MenuItemSlider* AddSlider_Internal(std::string text, float* pValue, float minValue, float maxValue, int decimals);

    void RemoveItem(MenuItem* item);
    void RemoveItems();

    void UpdateMenuTransform();

    bool NavigatePage(int add);

    void Close_Internal();

    Container* GetContainer();
    Container* GetContentContainer();

public:
    IMenuItem* AddCheckbox(const std::string& text, bool* pValue) override
    {
        auto item = AddCheckbox_Internal(text, pValue);
        return item;
    }

    IMenuItem* AddFloatOptions(const std::string& text, float* pValue, float min, float max, float step) override
    {
        auto item = AddFloatOptions_Internal(text, pValue, min, max, step);
        return item;
    }

    IMenuItem* AddIntOptions(const std::string& text, int* pValue, int min, int max, int step) override
    {
        auto item = AddIntOptions_Internal(text, pValue, min, max, step);
        return item;
    }

    IMenuItem* AddOptions(const std::string& text, float optionsWidth) override
    {
        auto item = AddOptions_Internal(text, optionsWidth);
        return item;
    }

    IMenuItem* AddItem(const std::string& text) override
    {
        auto item = AddItem_Internal(text);
        return item;
    }

    IMenuItem* AddButton(const std::string& text, std::function<void()> onClick) override
    {
        auto item = AddButton_Internal(text, onClick);
        return item;
    }

    IMenuItem* AddSlider(const std::string& text, float* pValue, float minValue, float maxValue, int decimals) override
    {
        auto item = AddSlider_Internal(text, pValue, minValue, maxValue, decimals);
        return item;
    }

    IMenuItem* AddCustomItem(const std::string& text, float height) override
    {
        auto item = AddItem_Internal(text);
        item->GetContainer()->style.height = std::to_string(height) + "px";
        return item;
    }

    void SetCloseButtonVisible(bool visible) override
    {
        _closeButton->visible = visible;
    }

    void Close() override
    {
        Close_Internal();
    }

    IWindow* OpenColorMenu(CRGBA* color) override;
};