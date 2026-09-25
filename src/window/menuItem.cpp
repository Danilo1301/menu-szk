#include "menuItem.h"

#include "../utils/eventListener.h"
#include "menu/menu.h"
#include "window.h"

#include "../container/containerLoader.h"

MenuItem::MenuItem(Window *window)
{
    LeakUtils::RegisterItem("MenuItem");

    onValueChange = new EventListener<>();

    container = window->GetContentContainer()->AddChild("item");
    LoadContainerFromFile(container, GetMenuLayoutPath("menu_item.json"));

    container->style.backgroundColorClicked = CRGBA(255, 255, 255, 180);

    auto title = container->FindChild("text");
    title->textFont.clickedColor = CRGBA(200, 200, 200);

    window->GetContentContainer()->UpdateTransform();

    IFont itemFont;
    itemFont.size = 1.2f;

    // container->canBlockTouchEvents = true;
    // container->style.localSize.x = 90;
    // container->style.localSize.y = 90;
    // container->text = "This is a item";
    // container->textFont = itemFont;
    // container->style.textOffset.x = 10;
    // container->style.textHorizontalAlign = HorizontalAlign::Left;
    // container->style.fillHorizontal = true;
    // container->style.backgroundColor = MenuColor(0, 0, 0, 0);
    // container->backgroundClickedColor = COLOR_WHITE;
    // container->textColor = COLOR_WHITE;
    // container->textClickedColor = COLOR_BLACK;
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

void MenuItem::AddColorPreview(CRGBA *color)
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