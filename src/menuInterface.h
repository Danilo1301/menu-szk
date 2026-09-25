#pragma once

#include "cellphone/cellphone.h"
#include "pch.h"

#include "menu/menu.h"

#include "peds.h"
#include "scriptsPanel.h"

#include "utils/eventListener.h"
#include "window/windowManager.h"
#include <functional>
#include <string>

class MenuInterface : public IMenuSZK
{
  public:
    IWindow *CreateWindow(float x, float y, float width, std::string title, std::string subtitle) override
    {
        auto window = WindowManager::CreateWindow(x, y, title, subtitle, width);
        return (IWindow *)window;
    }

    void AddCellphoneScript(std::string text, std::string iconPath, std::function<void()> fn) override
    {
        // ScriptsPanel::AddScriptFunction(text, fn);
        Cellphone::ScriptsCellphone->AddItem(text, iconPath, fn);
    }

    IEventListener<GameEntity> *onPedFound = new EventListener<GameEntity>();
    IEventListener<GameEntity> *onPedDestroy = new EventListener<GameEntity>();

    IEventListener<unsigned int> *onGameProcess = new EventListener<unsigned int>();
    IEventListener<unsigned int> *onPreRenderEnd = new EventListener<unsigned int>();

    std::vector<GameEntity> GetPeds() override { return Peds::GetPeds(); }
};

extern MenuInterface *menuInterface;

// MenuSZK* menuSZK = new MenuSZK();