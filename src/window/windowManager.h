#pragma once

#include "../pch.h"

#include "window.h"

class WindowManager
{
public:
    static std::vector<Window*> Windows;

    static Window* CreateWindow(float x, float y, const std::string& title, const std::string& subTitle, float width);
    static void SetToCloseWindow(Window* window);
    static void CloseRequestedWindows();
};