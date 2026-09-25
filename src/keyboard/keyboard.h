#pragma once

#include "../pch.h"
#include "../container/container.h"
#include <string>
#include <sys/stat.h>

class Keyboard {
private:
    static Container* mainContainer;
public:
    static bool IsVisible();
    static void CreateContainer();
    static void SetVisible(bool visible);
    static Container* AddButton(std::string text, CVector2D position, CVector2D size);
};