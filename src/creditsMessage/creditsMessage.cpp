#include "creditsMessage.h"
#include "menuSZK/imenuSZK.h"
#include "src/menus/menuSettings.h"
#include "src/pch.h"
#include "src/utils/drawUtils.h"

void CreditsMessage::Render()
{
    static IFontStyle font;
    static bool setupFont = false;

    if (!setupFont)
    {
        setupFont = true;

        font.align = GameFontAlignment::ALIGN_CENTER;
        font.size = 2.4f;
        font.color = COLOR_WHITE;
        font.dropColor = CRGBA(0, 0, 0);
        font.dropShadowPosition = 1;
    }

    if (menuSettings->GetBoolValue("top_screen_credits_message_enabled") == false)
    {
        //dont draw
        return;
    }

    auto text = menuSettings->GetStringValue("top_screen_credits_message");

    auto res = DrawUtils::GetBaseResolution();

    DrawUtils::DrawText(text, CVector2D(res.width / 2.0f, 80), font);
}