#include "m_menu.h"

#include "../utils/drawUtils.h"
#include "menu/menu.h"

void RenderExampleMenu()
{
    /*
    IFont fontMenuTitle;
    IFont fontItem;

    fontMenuTitle.fontSize = 4.0f;
    fontMenuTitle.align = MenuFontAlignment::ALIGN_CENTER;

    fontItem.fontSize = 2.0f;
    fontItem.align = MenuFontAlignment::ALIGN_LEFT;
    
    //

    auto baseRes = DrawUtils::GetBaseResolution();

    CRGBA titleColor = CRGBA(0x6C, 0x5E, 0xE5, 255);
    CRGBA blackColor = CRGBA(0x13, 0x14, 0x16, 255);
    CRGBA itemColor = CRGBA(0x13, 0x14, 0x16, 119);
    CRGBA selectedColor = CRGBA(0xCF, 0xD8, 0xE1, 255);
    CRGBA textColor = CRGBA(255, 255, 255, 255);

    const char* title = "Menu SZK";

    const char* items[] =
    {
        "Item 1",
        "Item 2",
        "Item 3",
        "Item 4",
        "Item 5",
        "Item 6"
    };

    float width = 650.0f;
    float titleHeight = 140.0f;
    float separatorHeight = 50.0f;
    float itemHeight = 70.0f;

    float top = 50.0f;
    float left = 300.0f;

    // Titulo
    DrawUtils::DrawRect(
        CVector2D(left, top),
        CVector2D(width, titleHeight),
        titleColor
    );

    DrawUtils::DrawText(
        title,
        CVector2D(left + width / 2, top + titleHeight / 2.0f),
        fontMenuTitle
    );

    // Rect preto abaixo do titulo
    float separatorTop = top + titleHeight;

    DrawUtils::DrawRect(
        CVector2D(left, separatorTop),
        CVector2D(width, separatorHeight),
        blackColor
    );

    // Itens
    for (int i = 0; i < 6; i++)
    {
        float itemTop = separatorTop + separatorHeight + i * itemHeight;

        CRGBA color = i == 1 ? selectedColor : itemColor;

        DrawUtils::DrawRect(
            CVector2D(left, itemTop),
            CVector2D(width, itemHeight),
            color
        );

        float marginLeft = 10;

        DrawUtils::DrawText(
            items[i],
            CVector2D(left + marginLeft, itemTop + itemHeight / 2.0f),
            fontItem
        );
    }
    */
}