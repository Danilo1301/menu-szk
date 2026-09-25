#pragma once

#include "../container/container.h"
#include "../pch.h"
#include <string>

class Window;

class MenuItem : public IMenuItem
{
  protected:
    Container *container = nullptr;

  public:
    MenuItem(Window *window);
    ~MenuItem();

    void SetTitle(std::string text);

    virtual bool GetBoolValue() override
    {
        LOGE("Cant get bool value from this menuItem");
        return false;
    }

    virtual void AddOption(int value, std::string displayText) override { LOGE("Cant add option to this menuItem"); }

    virtual int GetCurrentOptionValue() override
    {
        LOGE("Cant get current option value from this menuItem");
        return -1;
    }

    virtual IContainer *GetContainer() override { return container; }

    virtual void AddColorPreview(CRGBA *color) override;
};