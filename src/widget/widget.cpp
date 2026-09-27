#include "widget.h"
#include "../container/container.h"
#include "../container/containerLoader.h"
#include "menu/menu.h"
#include "src/pch.h"
#include "src/utils/eventListener.h"
#include <string>

Widget::Widget(Container *parent, std::string tag, std::string backgroundImage, std::string image)
{
    onClick = new EventListener<>();

    if (backgroundImage.empty())
    {
        backgroundImage = GetMenuAssetPath("widget/widget_background1.png");
    }

    LOGI("loading?");

    auto widget = parent->AddChild(tag);
    LoadContainerFromFile(widget, GetMenuLayoutPath("widget.json"));

    LOGI("loaded");

    auto icon = widget->FindChild("icon");

    widget->style.backgroundImage = backgroundImage;
    widget->hideWhenPaused = true;
    widget->canBlockTouchEvents = false;

    icon->style.backgroundImage = image;

    _container = widget;
    _icon = icon;

    widget->onClick->Add([this]() { onClick->Emit(); });

    widget->onStateChanged->Add(
        [icon](IContainerState state)
        {
            if (state == IContainerState::Clicked)
            {
                icon->style.imageColor = CRGBA(255, 0, 0);
            }
            else
            {
                icon->style.imageColor = CRGBA(255, 255, 255);
            }
        });

    widget->onPostUpdateTransform->Add(
        [this]()
        {
            if (_prevVisible != visible)
            {
                _prevVisible = visible;

                _container->visible = visible;
            }
        });
}

void Widget::SetPosition(float x, float y) { _container->SetRelativePosition(x, y); }

void Widget::SetSize(float size)
{
    _container->style.width = std::to_string(size) + "px";
    _container->style.height = std::to_string(size) + "px";
}

Widget *Widget::CreateWidget(float x, float y, float size, std::string bgImage, std::string image)
{
    auto widget = new Widget(Container::MainContainer, "widget", bgImage, image);
    widget->SetPosition(x, y);
    widget->SetSize(size);

    return widget;
}