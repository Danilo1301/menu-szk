#pragma once

#include "menuSZK/imenuSZK.h"

#include "../container/container.h"
#include "src/container/container.h"

class Widget : public IWidget
{
private:
    Container* _container;
    Container* _icon;

public:
    Widget(Container* parent, std::string tag, std::string backgroundImage, std::string image);
    ~Widget();

    void SetVisible(bool visible) override;
    void SetPosition(float x, float y) override;
    void SetSize(float size) override;
    float GetSize() override;
    void Destroy() override;
    IContainer* GetContainer() override;

    static Widget* CreateWidget(float x, float y, float size, std::string bgImage, std::string image);
    static void DestroyWidget(Widget* widget);
};