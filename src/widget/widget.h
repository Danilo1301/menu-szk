#pragma once

#include "menu/menu.h"

#include "../container/container.h"
#include "src/container/container.h"

class Widget : public IWidget
{
  private:
    Container *_container;
    Container *_icon;

    bool _prevVisible = true;

  public:
    Widget(Container *parent, std::string tag, std::string backgroundImage, std::string image);

    void SetPosition(float x, float y) override;
    void SetSize(float size) override;

    static Widget *CreateWidget(float x, float y, float size, std::string bgImage, std::string image);
};