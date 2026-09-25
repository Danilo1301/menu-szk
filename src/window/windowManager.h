#pragma once

#include "../pch.h"

#include "window.h"

class WindowManager {
public:
    static std::vector<Window*> Windows;

    static Window* CreateWindow(float x, float y, std::string title, std::string subTitle, float width);
    static void SetToCloseWindow(Window* window);
    static void CloseRequestedWindows();
};