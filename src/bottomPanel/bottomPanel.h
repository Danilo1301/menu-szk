#pragma once

#include "../container/container.h"
#include "../pch.h"
#include "menu/menu.h"

#include <functional>
#include <string>

class BottomPanel
{
  public:
    struct Item
    {
        std::string title;
        std::string imagePath;
        std::function<void()> callback;
    };

    static BottomPanel *Main;
    static BottomPanel *CreateMain();

  private:
    std::vector<Item> _items;
    Container *_container;

  public:
    BottomPanel();

    void SetVisible(bool visible);
    void FadeIn();
    void FadeOut();

    void AddItem(std::string title, std::string imagePath, std::function<void()> callback);

    bool IsVisible();
};