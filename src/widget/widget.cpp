#include "widget.h"
#include "../container/container.h"
#include "../container/containerLoader.h"
#include "menuSZK/imenuSZK.h"
#include "src/container/cssValue.h"
#include "src/logHelper.h"
#include "src/utils/eventListener.h"
#include "../logHelper.h"
#include <string>
#include <vector>

std::vector<Widget*> _widgetsToDestroy;

Widget::Widget(Container* parent, std::string tag, std::string backgroundImage, std::string image)
{
    onClick = new EventListener<>();

    if (backgroundImage.empty()) { backgroundImage = GetMenuAssetPath("widget/widget_background1.png"); }

    auto widget = parent->AddChild(tag);
    LoadContainerFromFile(widget, GetMenuLayoutPath("widget.json"));

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
            if (state == IContainerState::Clicked) { icon->style.imageColor = CRGBA(255, 0, 0); }
            else
            {
                icon->style.imageColor = CRGBA(255, 255, 255);
            }
        });
}

Widget::~Widget()
{
    _container->Destroy();
    _container = nullptr;

    delete onClick;
    onClick = nullptr;
}

void Widget::SetVisible(bool visible)
{
    //if (visible != _container->visible) { LOGW("visibility of widget has changed"); }

    _container->visible = visible;
}

void Widget::SetPosition(float x, float y)
{
    _container->SetRelativePosition(x, y);
}

void Widget::SetSize(float size)
{
    _container->style.width = std::to_string(size) + "px";
    _container->style.height = std::to_string(size) + "px";
}

float Widget::GetSize()
{
    return CSSValue::StaticParse(_container->style.width, 0);
}

void Widget::Destroy()
{
    _widgetsToDestroy.push_back(this);
}

IContainer* Widget::GetContainer()
{
    return _container;
}

Widget* Widget::CreateWidget(float x, float y, float size, std::string bgImage, std::string image)
{
    auto widget = new Widget(Container::MainContainer, "widget", bgImage, image);
    widget->SetPosition(x, y);
    widget->SetSize(size);

    return widget;
}

void Widget::DestroyWidgetsThatNeedsToBeDestroyed()
{
    BEGIN_OPERATION_DESC(op_Test, "Destroying widgets");

    for (Widget* widget : _widgetsToDestroy) { delete widget; }

    _widgetsToDestroy.clear();

    END_OPERATION(op_Test);
}