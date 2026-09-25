#include "menuItemCheckbox.h"

// #include "mod/logger.h"
#include "../../config.h"
#include "aml-psdk/gta_base/Vector.h"
#include "menu/menu.h"
#include <cstddef>
#include <functional>

#include "../../container/containerLoader.h"

MenuItemCheckbox::MenuItemCheckbox(Window *window, bool *ptr) : MenuItem(window)
{
    this->pBool = ptr;

    container->onClick->Add(
        [this]()
        {
            *pBool = !*pBool;

            logger->Info("Value is now: %i", *pBool);

            UpdateCheckbox();

            onValueChange->Emit();
        });

    {
        checkboxContainer = container->AddChild("checkbox");
        LoadContainerFromFile(checkboxContainer, GetMenuLayoutPath("checkbox.json"));

        UpdateCheckbox();
    }
}

MenuItemCheckbox::~MenuItemCheckbox() {}

void MenuItemCheckbox::UpdateCheckbox()
{
    bool on = *pBool;

    if (on)
    {
        checkboxContainer->style.backgroundImage = GetMenuAssetPath("menu/checkbox_on.png");
    }
    else
    {
        checkboxContainer->style.backgroundImage = GetMenuAssetPath("menu/checkbox_off.png");
    }
}
