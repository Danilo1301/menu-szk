#pragma once

#include <string>

#include "aml-psdk/gta_base/Vector.h"
#include "../utils/eventListener.h"

class Container;
class Keyboard
{
public:
    static EventListener<std::string>* OnEnterPressed;

    static bool IsVisible();

    static void Open(std::string value, KeyboardFlags flags = KeyboardFlags::None);
    static void Close();

    static void SetVisible(bool visible);

    static void CreateContainer();
    static Container* AddButton(std::string text, CVector2D position, CVector2D size);

private:
    static void OnKeyPressed(const std::string& key);
    static void FormatInput();
    static void UpdateInputText();

    static Container* mainContainer;
    static std::string _input;
    static bool _capslockOn;
    static KeyboardFlags _flags;
};