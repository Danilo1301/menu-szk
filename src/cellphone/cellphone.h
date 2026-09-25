#pragma once

#include "../pch.h"
#include <functional>
#include <string>

class Container;

struct CellphoneItem
{
    std::string image;
    std::string text;
    std::function<void()> function;
};

class Cellphone
{
  private:
    Container *_container = nullptr;
    Container *_contentPanel = nullptr;
    Container *_pageInfo = nullptr;
    Container *_appsInfo = nullptr;

    std::vector<CellphoneItem> _items;

    int _page = 0;

    void UpdatePage();

  public:
    static Cellphone *ScriptsCellphone;
    static Cellphone *CreateScriptsCellphone();

    Cellphone();
    ~Cellphone();

    void AddItem(std::string text, std::string image, std::function<void()> function);

    void SetVisible(bool visible);
    void FadeIn();
    void FadeOut();
};