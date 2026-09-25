#include "windowManager.h"
#include "aml-psdk/gta_base/Vector.h"
#include "mod/logger.h"
#include <sys/stat.h>

#include "../utils/effects.h"

std::vector<Window *> WindowsToClose;

std::vector<Window *> WindowManager::Windows;

Window *WindowManager::CreateWindow(float x, float y, std::string title, std::string subTitle, float width)
{
    logger->Info("Creating window");

    auto window = new Window();
    window->width = width;
    window->title = title;
    window->subTitle = subTitle;

    Windows.push_back(window);

    window->Init(x, y);

    // TestEase(
    //     window->GetContainer(), CVector2D(0, 0), CVector2D(x, y), CVector2D(0, 0), CVector2D(1, 1), 0.0f, 1.0f,
    //     1000);

    //

    return window;
}

void WindowManager::SetToCloseWindow(Window *window)
{
    if (window->isClosing)
        return;

    logger->Info("Window requested to be closed: %s", window->title.c_str());

    window->isClosing = true;

    WindowsToClose.push_back(window);
}

void WindowManager::CloseRequestedWindows()
{
    for (Window *window : WindowsToClose)
    {
        auto it = std::find(Windows.begin(), Windows.end(), window);

        logger->Info("Deleting window: %s", window->title.c_str());

        delete *it;

        logger->Info("Deleted window");

        Windows.erase(it);
    }

    WindowsToClose.clear();
}