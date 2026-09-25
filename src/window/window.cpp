#include "window.h"

#include "../config.h"

#include "aml-psdk/gta_base/Vector.h"

#include "menu/menu.h"
#include "menuItem.h"
#include "menuItems/menuItemCheckbox.h"
#include "menuItems/menuItemOptions.h"

#include "mod/logger.h"
#include "windowManager.h"
#include <functional>
#include <stdio.h>
#include <string>
#include <unistd.h>

#include "../utils/eventListener.h"

#include "../utils/effects.h"

#include "../container/containerLoader.h"

Window::Window()
{
    LeakUtils::RegisterItem("Window");

    onClose = new EventListener<>();
}

Window::~Window()
{
    LeakUtils::FreeItem("Window");

    RemoveItems();

    delete _container;
}

void Window::Init(float x, float y)
{
    bool useEffect = true;
    auto windowStartPosition = CVector2D(x, y);

    if (useEffect)
    {
        windowStartPosition = CVector2D(0, 0);
    }

    IFont titleFont;
    titleFont.size = 4.0f;
    titleFont.align = MenuFontAlignment::ALIGN_CENTER;

    IFont subtitleFont;
    subtitleFont.size = 2.0f;

    IFont btnFont;
    btnFont.size = 4.0f;
    btnFont.align = MenuFontAlignment::ALIGN_CENTER;

    {
        auto container = Container::MainContainer->AddChild("");
        LoadContainerFromFile(container, GetMenuLayoutPath("menu.json"));

        container->SetRelativePosition(windowStartPosition.x, windowStartPosition.y);

        _container = container;
        _content = container->FindChild("content");
        _title = container->FindChild("title");
        _subTitle = container->FindChild("subtitle");

        container->canBlockTouchEvents = true;
        container->canDrag = true;

        container->onPreUpdateTransform->Add([this]() { UpdateMenuTransform(); });

        _title->onPostUpdateTransform->Add([this]() { _title->style.imageColor = windowColor; });
        _title->text = title;
        _subTitle->text = subTitle;

        //_content->drawBoundings = true;
    }

    {
        auto btn = _container->FindChild("leftBtn");

        _leftButton = btn;

        btn->canBlockTouchEvents = true;

        btn->onClick->Add([this]() { NavigatePage(-1); });

        btn->onStateChanged->Add(
            [btn](IContainerState state)
            {
                auto disabled = GetMenuAssetPath("menu/invisible.png");
                auto normal = GetMenuAssetPath("menu/button_left.png");
                auto clicked = GetMenuAssetPath("menu/button_left_off.png");

                if (state == IContainerState::Normal)
                {
                    btn->style.backgroundImage = normal;
                }
                if (state == IContainerState::Clicked)
                {
                    btn->style.backgroundImage = clicked;
                }
                if (state == IContainerState::Disabled)
                {
                    btn->style.backgroundImage = disabled;
                }
            });
    }

    {
        auto btn = _container->FindChild("rightBtn");

        _rightButton = btn;

        btn->canBlockTouchEvents = true;

        btn->onClick->Add([this]() { NavigatePage(1); });

        btn->onStateChanged->Add(
            [btn](IContainerState state)
            {
                auto disabled = GetMenuAssetPath("menu/invisible.png");
                auto normal = GetMenuAssetPath("menu/button_right.png");
                auto clicked = GetMenuAssetPath("menu/button_right_off.png");

                if (state == IContainerState::Normal)
                {
                    btn->style.backgroundImage = normal;
                }
                if (state == IContainerState::Clicked)
                {
                    btn->style.backgroundImage = clicked;
                }
                if (state == IContainerState::Disabled)
                {
                    btn->style.backgroundImage = disabled;
                }
            });
    }

    {
        auto container = _container->FindChild("closeBtn");

        container->canBlockTouchEvents = true;
        container->onClick->Add([this]() { Close(); });
    }

    const CVector2D startPosition = windowStartPosition;
    const CVector2D endPosition = CVector2D(x, y);
    const CVector2D startScale(0.0f, 0.0f);
    const CVector2D endScale(1.0f, 1.0f);
    const float startOpacity = 0.0f;
    const float endOpacity = 1.0f;
    const int duration = 500;
    const auto onComplete = [this]() {};

    _container->style.opacity = 0;
    _container->style.scale = startScale;
    _container->UpdateTransform();

    Ease_Curve(
        _container, startPosition, endPosition, startScale, endScale, startOpacity, endOpacity, duration, onComplete);
}

template <typename T> T *Window::AddItem_T(T *item)
{
    items.push_back(item);

    return item;
}

MenuItem *Window::AddItemDefault() { return AddItem_T(new MenuItem(this)); }

MenuItemCheckbox *Window::AddCheckbox_Internal(std::string text, bool *ptr)
{
    auto item = AddItem_T(new MenuItemCheckbox(this, ptr));
    item->SetTitle(text);
    return item;
}

MenuItemOptions *Window::AddOptions_Internal(std::string text, float optionsWidth)
{
    auto item = AddItem_T(new MenuItemOptions(this, optionsWidth));
    item->SetTitle(text);
    return item;
}

MenuItemOptions *Window::AddIntOptions_Internal(std::string text, int *ptr, int min, int max, int step)
{
    auto item = AddOptions_Internal(text);
    item->SetInteger(ptr, min, max, step);

    return item;
}

MenuItemOptions *Window::AddFloatOptions_Internal(std::string text, float *ptr, float min, float max, float step)
{
    auto item = AddOptions_Internal(text);
    item->SetFloat(ptr, min, max, step);

    return item;
}

MenuItem *Window::AddItem_Internal(std::string text)
{
    auto item = AddItem_T(new MenuItem(this));
    item->SetTitle(text);
    return item;
}

MenuItem *Window::AddButton_Internal(std::string text, std::function<void()> onClick)
{
    auto item = AddItem_T(new MenuItem(this));
    item->SetTitle(text);
    item->GetContainer()->onClick->Add([onClick]() { onClick(); });
    return item;
}

MenuItemSlider *Window::AddSlider_Internal(
    std::string text, float *pValue, float minValue, float maxValue, int decimals)
{
    auto item = AddItem_T(new MenuItemSlider(this, pValue, minValue, maxValue, decimals));
    item->SetTitle(text);
    return item;
}

void Window::RemoveItem(MenuItem *item)
{
    if (item == nullptr)
        return;

    auto it = std::find(items.begin(), items.end(), item);

    if (it == items.end())
        return;

    items.erase(it);

    delete item;
}

void Window::RemoveItems()
{
    for (MenuItem *item : items)
    {
        delete item;
    }

    items.clear();
}

void Window::UpdateMenuTransform()
{
    bool containerIsBlocked = _container->block != IBlockType::None;
    // true

    if (containerIsBlocked != blocked)
    {
        _container->SetBlocked(blocked);

        //_container->block = blocked ? IBlockType::BlockThisAndChildren : IBlockType::None;
    }

    const int firstItem = currentPage * maxItemsPerPage;
    const int lastItem = firstItem + maxItemsPerPage;

    const float titleHeight = CSSValue::StaticParse(_title->style.height, 0);
    const float subtitleHeight = CSSValue::StaticParse(_subTitle->style.height, 0);

    float itemY = 0;

    for (int i = 0; i < items.size(); i++)
    {
        MenuItem *item = items[i];

        auto itemContainer = (Container *)item->GetContainer();

        if (i < firstItem || i >= lastItem)
        {
            itemContainer->visible = false;
            continue;
        }

        itemContainer->visible = true;
        itemContainer->SetRelativePosition(0, itemY);

        float itemHeight = itemContainer->GetCurrentSize().y;
        // float itemHeight = 100;

        itemY += itemHeight;
    }

    _container->style.height = std::to_string(titleHeight + subtitleHeight + itemY) + "px";

    //

    const int totalPages = (items.size() + maxItemsPerPage - 1) / maxItemsPerPage;

    _leftButton->SetDisabled(currentPage <= 0);
    _rightButton->SetDisabled(currentPage >= totalPages - 1);
}

bool Window::NavigatePage(int add)
{
    const int totalPages = (items.size() + maxItemsPerPage - 1) / maxItemsPerPage;

    const int newPage = currentPage + add;

    if (newPage < 0 || newPage >= totalPages)
        return false;

    currentPage = newPage;

    UpdateMenuTransform();

    _container->UpdateTransformFromRoot();

    return true;
}

void Window::Close_Internal()
{
    // auto startPos = GetContainer()->style.localPosition;
    auto startPos = CVector2D(500, 500);

    logger->Info("startPos = %f, %f", startPos.x, startPos.y);

    onClose->Emit();

    const CVector2D startPosition = _container->GetCenterPosition();
    const CVector2D endPosition = CVector2D(0, 0);
    const CVector2D startScale(1.0f, 1.0f);
    const CVector2D endScale(0.0f, 0.0f);
    const float startOpacity = 1.0f;
    const float endOpacity = 0.0f;
    const int duration = 500;
    const auto onComplete = [this]() { WindowManager::SetToCloseWindow(this); };

    Ease_Curve(
        _container, startPosition, endPosition, startScale, endScale, startOpacity, endOpacity, duration, onComplete);
}

Container *Window::GetContainer() { return _container; }
Container *Window::GetContentContainer() { return _content; }
