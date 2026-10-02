#include "menuSZK.h"
#include "keyboard/keyboard.h"
#include "src/utils/eventListener.h"

MenuSZK::MenuSZK()
{
    onPedAdded = new EventListener<GameEntity>();
    onPedRemoved = new EventListener<GameEntity>();
    onVehicleAdded = new EventListener<GameEntity>();
    onVehicleRemoved = new EventListener<GameEntity>();
    onPlayerReady = new EventListener<>();

    onMenuProcess = new EventListener<unsigned int>();
    onScriptProcess = new EventListener<unsigned int>();

    onDrawBeforeMenu = new EventListener<unsigned int>();
    onDrawAfterMenu = new EventListener<unsigned int>();

    onPostDrawRadar = new EventListener<>();

    onKeyboardResult = new EventListener<std::string>();
    Keyboard::OnEnterPressed->Add([this](std::string input) { onKeyboardResult->Emit(input); });
}

MenuSZK* menuSZK = new MenuSZK();