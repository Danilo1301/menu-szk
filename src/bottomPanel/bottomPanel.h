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
    bool _isFading = false;
    Container *_container;
    unsigned int _timeOpened = 0;

    std::vector<Container *> _itemContainers;

  public:
    BottomPanel();

    void SetVisible(bool visible);
    void FadeIn();
    void FadeOut();

    void AddItem(std::string title, std::string imagePath, std::function<void()> callback);
    void UpdateItemsLayout();

    bool IsVisible();
    bool IsPointerInside(int trackId);
    int GetTimeVisible();
};