#include "menuItem.h"

#include "../utils/eventListener.h"
#include "menuSZK/imenuSZK.h"
#include "window.h"

#include "../container/containerLoader.h"

MenuItem::MenuItem(Window* window)
{
    LeakUtils::RegisterItem("MenuItem");

    onValueChange = new EventListener<>();

    container = window->GetContentContainer()->AddChild("item");
    LoadContainerFromFile(container, GetMenuLayoutPath("menu_item.json"));

    container->style.backgroundColorClicked = CRGBA(255, 255, 255, 180);

    auto title = container->FindChild("text");

    window->GetContentContainer()->UpdateTransform();
}

MenuItem::~MenuItem()
{
    LeakUtils::FreeItem("MenuItem");

    delete container;
}

void MenuItem::SetTitle(std::string text)
{
    auto text_container = container->FindChild("text");
    text_container->text = text;
}

void MenuItem::AddColorPreview(CRGBA* color)
{
    auto colorPreview = container->AddChild("colorPreview");
    colorPreview->style.position = "absolute";
    colorPreview->style.right = "0px";
    colorPreview->style.width = "90px";
    colorPreview->style.top = "5%";
    colorPreview->style.right = "10px";
    colorPreview->style.height = "90%";
    colorPreview->style.backgroundImage = GetMenuAssetPath("colorpicker/preview.png");
    colorPreview->style.imageColor = *color;
    colorPreview->onPostUpdateTransform->Add([colorPreview, color]() { colorPreview->style.imageColor = *color; });
}

void MenuItem::AddIcon(const std::string& pngFilePath)
{
    auto image = container->AddChild("image");
    image->style.backgroundImage = pngFilePath;
    image->style.width = "90px";
    image->style.height = "90px";

    auto text_container = container->FindChild("text");
    text_container->style.margin.left = 100;
}