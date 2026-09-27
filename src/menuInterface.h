#pragma once

#include "pch.h"

#include "aml-psdk/gta_base/RGBA.h"
#include "cellphone/cellphone.h"
#include "menu/menu.h"
#include "peds.h"
#include "src/container/container.h"
#include "src/utils/textureLoader.h"
#include "utils/eventListener.h"
#include "vehicles.h"
#include "widget/widget.h"
#include "window/windowManager.h"
#include <cstddef>
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

    IEventListener<GameEntity> *onPedAdded = new EventListener<GameEntity>();
    IEventListener<GameEntity> *onPedRemoved = new EventListener<GameEntity>();
    IEventListener<GameEntity> *onVehicleAdded = new EventListener<GameEntity>();
    IEventListener<GameEntity> *onVehicleRemoved = new EventListener<GameEntity>();

    IEventListener<unsigned int> *onGameProcess = new EventListener<unsigned int>();
    IEventListener<unsigned int> *onScriptProcess = new EventListener<unsigned int>();
    IEventListener<unsigned int> *onPreRenderEnd = new EventListener<unsigned int>();
    IEventListener<> *onPostDrawRadar = new EventListener<>();

    std::vector<GameEntity> GetPeds() override { return Peds::GetPeds(); }
    std::vector<GameEntity> GetVehicles() override { return Vehicles::GetVehicles(); }

    IWidget *CreateWidget(float x, float y, float size, std::string bgImage, std::string image) override
    {
        LOGI("here its ok");

        auto widget = Widget::CreateWidget(x, y, size, bgImage, image);

        return (IWidget *)widget;
    }

    RwTexture *LoadTexture(std::string pngFilePath, bool cache = true) override
    {
        RwTexture *ptr = nullptr;

        if (cache)
        {
            ptr = LoadRwTextureFromFileAndCache(pngFilePath, "texture", false, COLOR_WHITE);
        }
        else
        {
            ptr = LoadRwTextureFromFile(pngFilePath, "texture", false, COLOR_WHITE);
        }

        return ptr;
    }

    virtual IContainer *GetMainContainer() override { return (IContainer *)Container::MainContainer; }
};

extern MenuInterface *menuInterface;

// MenuSZK* menuSZK = new MenuSZK();