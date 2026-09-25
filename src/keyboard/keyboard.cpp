#include "keyboard.h"

#include "../config.h"
#include "../container/container.h"
#include "../utils/drawUtils.h"
#include "../utils/textureLoader.h"

#include "aml-psdk/game_sa/utils/OpcodeCallerIDs.h"
#include "aml-psdk/gta_base/Vector.h"
#include "menu/menu.h"

bool _isKeyboardVisible = false;

Container *Keyboard::mainContainer = nullptr;

static constexpr float KEYBOARD_REFERENCE_WIDTH = 1159.0f;
static constexpr float KEYBOARD_REFERENCE_HEIGHT = 558.0f;

bool Keyboard::IsVisible() { return _isKeyboardVisible; }

void Keyboard::CreateContainer()
{
    auto screenResolution = DrawUtils::GetBaseResolution();

    // Tamanho e posição do teclado na tela.
    CVector2D keyboardPosition(0.0f, screenResolution.height - 600.0f);

    CVector2D keyboardSize(screenResolution.width, 600.0f);

    mainContainer = Container::MainContainer->AddChild("keyboard");

    mainContainer->canBlockTouchEvents = false;

    mainContainer->style.left = std::to_string(keyboardPosition.x) + "px";
    mainContainer->style.top = std::to_string(keyboardPosition.y) + "px";

    mainContainer->style.width = std::to_string(keyboardSize.x) + "px";
    mainContainer->style.height = std::to_string(keyboardSize.y) + "px";

    mainContainer->style.right = "auto";
    mainContainer->style.bottom = "auto";

    // Escala independente em X/Y para preencher
    // exatamente o tamanho configurado do teclado.
    const float scaleX = keyboardSize.x / KEYBOARD_REFERENCE_WIDTH;

    const float scaleY = keyboardSize.y / KEYBOARD_REFERENCE_HEIGHT;

    auto AddKey = [scaleX, scaleY](const char *text, float x, float y, float width, float height)
    {
        return Keyboard::AddButton(text, CVector2D(x * scaleX, y * scaleY), CVector2D(width * scaleX, height * scaleY));
    };

    // ---------------------------------------------------------
    // NUMBERS
    // ---------------------------------------------------------

    AddKey("1", 8, 3, 99, 92);
    AddKey("2", 122, 3, 99, 92);
    AddKey("3", 237, 3, 99, 92);
    AddKey("4", 352, 3, 99, 92);
    AddKey("5", 467, 3, 99, 92);
    AddKey("6", 582, 3, 99, 92);
    AddKey("7", 697, 3, 99, 92);
    AddKey("8", 812, 3, 99, 92);
    AddKey("9", 927, 3, 99, 92);
    AddKey("0", 1045, 3, 99, 92);

    // ---------------------------------------------------------
    // QWERTY
    // ---------------------------------------------------------

    AddKey("Q", 8, 109, 99, 92);
    AddKey("W", 122, 109, 99, 92);
    AddKey("E", 237, 109, 99, 92);
    AddKey("R", 352, 109, 99, 92);
    AddKey("T", 467, 109, 99, 92);
    AddKey("Y", 582, 109, 99, 92);
    AddKey("U", 697, 109, 99, 92);
    AddKey("I", 812, 109, 99, 92);
    AddKey("O", 927, 109, 99, 92);
    AddKey("P", 1045, 109, 99, 92);

    // ---------------------------------------------------------
    // ASDF
    // ---------------------------------------------------------

    AddKey("A", 66, 215, 100, 93);
    AddKey("S", 180, 215, 100, 93);
    AddKey("D", 294, 215, 100, 93);
    AddKey("F", 408, 215, 100, 93);
    AddKey("G", 523, 215, 100, 93);
    AddKey("H", 638, 215, 100, 93);
    AddKey("J", 753, 215, 100, 93);
    AddKey("K", 868, 215, 100, 93);
    AddKey("L", 983, 215, 100, 93);

    // ---------------------------------------------------------
    // SHIFT + ZXCVBNM + BACKSPACE
    // ---------------------------------------------------------

    AddKey("SHIFT", 8, 321, 158, 93);

    AddKey("Z", 180, 321, 99, 93);
    AddKey("X", 294, 321, 99, 93);
    AddKey("C", 409, 321, 99, 93);
    AddKey("V", 524, 321, 99, 93);
    AddKey("B", 639, 321, 99, 93);
    AddKey("N", 754, 321, 99, 93);
    AddKey("M", 869, 321, 99, 93);

    AddKey("BACKSPACE", 985, 321, 159, 93);

    // ---------------------------------------------------------
    // BOTTOM
    // ---------------------------------------------------------

    AddKey("CTRL", 8, 427, 158, 126);
    AddKey("ALT", 180, 427, 99, 126);
    AddKey("TAB", 294, 427, 99, 126);

    AddKey("SPACE", 408, 427, 447, 126);

    AddKey(".", 869, 427, 99, 126);

    auto enter = AddKey("ENTER", 985, 427, 159, 126);
    enter->onClick->Add([]() { Keyboard::SetVisible(false); });
}

void Keyboard::SetVisible(bool visible)
{
    if (visible && !mainContainer)
    {
        CreateContainer();
    }

    if (mainContainer)
    {
        mainContainer->visible = visible;
    }
}

Container *Keyboard::AddButton(std::string text, CVector2D position, CVector2D size)
{
    auto container = mainContainer->AddChild("button-" + text);

    container->style.left = std::to_string(position.x) + "px";
    container->style.top = std::to_string(position.y) + "px";
    container->style.width = std::to_string(size.x) + "px";
    container->style.height = std::to_string(size.y) + "px";
    container->text = text;
    container->style.textHorizontalAlign = HorizontalAlign::Middle;
    container->style.textVerticalAlign = VerticalAlign::Middle;
    container->textFont.size = 1.5f;
    container->textFont.align = MenuFontAlignment::ALIGN_CENTER;
    container->textFont.style = MenuFontStyle::FONT_SUBTITLES;

    auto pngFile = GetMenuAssetPath("menu/keyboard_button.png");
    auto pngFileDown = GetMenuAssetPath("menu/keyboard_button_down.png");

    container->style.backgroundImage = pngFile;

    container->onStateChanged->Add(
        [container, pngFile, pngFileDown](IContainerState state)
        {
            if (state == IContainerState::Clicked)
            {
                container->SetBackgroundImageIgnoreStyle(pngFileDown);
            }
            if (state == IContainerState::Normal)
            {
                container->SetBackgroundImageIgnoreStyle(pngFile);
            }
        });

    container->canBlockTouchEvents = true;

    container->onClick->Add(
        [container]()
        {
            // container->BeginClickedVisuals(0.5f);
        });

    container->onStateChanged->Add(
        [container](IContainerState state)
        {
            auto normal = GetMenuAssetPath("menu/keyboard_button.png");
            auto clicked = GetMenuAssetPath("menu/keyboard_button_down.png");

            if (state == IContainerState::Normal)
            {
                container->style.backgroundImage = normal;
            }
            if (state == IContainerState::Clicked)
            {
                container->style.backgroundImage = clicked;
            }
        });

    return container;
}