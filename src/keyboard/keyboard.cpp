#include "keyboard.h"

#include "../config.h"
#include "../container/container.h"

#include "aml-psdk/gta_base/Vector.h"
#include "mod/logger.h"
#include "src/utils/eventListener.h"
#include <functional>

bool _isKeyboardVisible = false;

Container* Keyboard::mainContainer = nullptr;
std::string Keyboard::_input;
bool Keyboard::_capslockOn = false;
KeyboardFlags Keyboard::_flags = KeyboardFlags::None;

//static constexpr float KEYBOARD_REFERENCE_WIDTH = 1159.0f;
//static constexpr float KEYBOARD_REFERENCE_HEIGHT = 558.0f;

EventListener<std::string>* Keyboard::OnEnterPressed = new EventListener<std::string>();

bool Keyboard::IsVisible()
{
    return _isKeyboardVisible;
}

void Keyboard::Open(std::string value, KeyboardFlags flags)
{
    _input = value;
    _flags = flags;
    SetVisible(true);
    UpdateInputText();
}

void Keyboard::Close()
{
    SetVisible(false);
}

void Keyboard::CreateContainer()
{
    auto keyboard = Container::MainContainer->AddChild("keyboard");

    mainContainer = keyboard;

    auto getKeyboardImage = [](std::string image) { return GetMenuAssetPath(image); };

    keyboard->canBlockTouchEvents = true;

    keyboard->style.backgroundImage = getKeyboardImage("keyboard/background.png");
    keyboard->style.left = "50%";
    keyboard->style.top = "50%";
    keyboard->style.width = "60%";
    keyboard->style.height = "60%";
    keyboard->style.transformOrigin = CVector2D(0, 0);

    auto* inputContainer = keyboard->AddChild("inputContainer");

    inputContainer->style.top = "-20%";
    inputContainer->style.height = "20%";
    inputContainer->style.width = "100%";
    inputContainer->style.backgroundImage = getKeyboardImage("keyboard/inputBackground.png");
    inputContainer->style.textHorizontalAlign = HorizontalAlign::Middle;
    inputContainer->style.textVerticalAlign = VerticalAlign::Middle;
    inputContainer->text = "";
    inputContainer->fontStyle.size = 2;

    const float gap = 1.0f;

    auto createButton =
        [&](float left, float top, float width, float height, const std::string& text, const std::string& bg, const std::string& key)
    {
        auto button = keyboard->AddChild("button");

        button->style.left = std::to_string(left) + "%";
        button->style.top = std::to_string(top) + "%";
        button->style.width = std::to_string(width) + "%";
        button->style.height = std::to_string(height) + "%";
        button->style.backgroundImage = getKeyboardImage("keyboard/" + bg + ".png");

        button->style.textHorizontalAlign = HorizontalAlign::Middle;
        button->style.textVerticalAlign = VerticalAlign::Middle;

        button->text = text;

        button->fontStyle.size = 2;
        button->fontStyle.align = GameFontAlignment::ALIGN_CENTER;

        button->onClick->Add([key]() { OnKeyPressed(key); });

        button->onStateChanged->Add(
            [&, button](IContainerState state)
            {
                logger->Info("1");

                if (state == IContainerState::Clicked) { button->style.imageColor = CRGBA(40, 40, 40); }
                else
                {
                    button->style.imageColor = CRGBA(255, 255, 255);
                }

                logger->Info("2");
            });

        return button;
    };

    const int columns = 10;
    const int rows = 5;

    const float keyWidth = (100.0f - gap * (columns + 1)) / columns;
    const float keyHeight = (100.0f - gap * (rows + 1)) / rows;

    auto getColumnLeft = [&](int column) { return gap + column * (keyWidth + gap); };

    auto getRowTop = [&](int row) { return gap + row * (keyHeight + gap); };

    // ---------------------------------------------------------
    // Numbers
    // ---------------------------------------------------------

    const std::vector<std::string> numbers = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" };

    for (int i = 0; i < numbers.size(); i++)
    {
        createButton(getColumnLeft(i), getRowTop(0), keyWidth, keyHeight, numbers[i], "key2", numbers[i]);
    }

    // ---------------------------------------------------------
    // QWERTY
    // ---------------------------------------------------------

    const std::vector<std::string> lettersRow1 = { "q", "w", "e", "r", "t", "y", "u", "i", "o", "p" };

    for (int i = 0; i < lettersRow1.size(); i++)
    {
        createButton(getColumnLeft(i), getRowTop(1), keyWidth, keyHeight, lettersRow1[i], "key1", lettersRow1[i]);
    }

    // ---------------------------------------------------------
    // ASDF
    // ---------------------------------------------------------

    const std::vector<std::string> lettersRow2 = { "a", "s", "d", "f", "g", "h", "j", "k", "l", "." };

    for (int i = 0; i < lettersRow2.size(); i++)
    {
        createButton(getColumnLeft(i), getRowTop(2), keyWidth, keyHeight, lettersRow2[i], "key1", lettersRow2[i]);
    }

    // ---------------------------------------------------------
    // Bottom letters
    // ---------------------------------------------------------

    createButton(getColumnLeft(0), getRowTop(3), keyWidth, keyHeight, "", "lshift", "SHIFT");

    const std::vector<std::string> lettersRow3 = { "z", "x", "c", "v", "b", "n", "m", "," };

    for (int i = 0; i < lettersRow3.size(); i++)
    {
        createButton(getColumnLeft(i + 1), getRowTop(3), keyWidth, keyHeight, lettersRow3[i], "key1", lettersRow3[i]);
    }

    createButton(getColumnLeft(9), getRowTop(3), keyWidth, keyHeight, "", "return", "BACKSPACE");

    // ---------------------------------------------------------
    // Bottom row
    // ---------------------------------------------------------

    const float bottomTop = getRowTop(4);
    const float bottomColumnWidth = (100.0f - gap * 11) / 10;

    auto bottomLeft = [&](int column) { return gap + column * (bottomColumnWidth + gap); };

    auto bottomWidth = [&](int columnsCount) { return bottomColumnWidth * columnsCount + gap * (columnsCount - 1); };

    createButton(bottomLeft(0), bottomTop, bottomWidth(1), keyHeight, "", "key1", "");

    createButton(bottomLeft(1), bottomTop, bottomWidth(1), keyHeight, "", "key1", "");

    createButton(bottomLeft(2), bottomTop, bottomWidth(1), keyHeight, "", "key1", "");

    createButton(bottomLeft(3), bottomTop, bottomWidth(4), keyHeight, "SPACE", "key1", "SPACE");

    createButton(bottomLeft(7), bottomTop, bottomWidth(1), keyHeight, "-", "key1", "-");

    createButton(bottomLeft(8), bottomTop, bottomWidth(1), keyHeight, "_", "key1", "_");

    createButton(bottomLeft(9), bottomTop, bottomWidth(1), keyHeight, "", "enter", "ENTER");
}

void Keyboard::SetVisible(bool visible)
{
    if (!mainContainer) { CreateContainer(); }

    if (visible && !mainContainer) { CreateContainer(); }

    if (mainContainer) { mainContainer->visible = visible; }

    _isKeyboardVisible = visible;
}

Container* Keyboard::AddButton(std::string text, CVector2D position, CVector2D size)
{
    auto* container = mainContainer->AddChild("button-" + text);

    return container;
}

void Keyboard::OnKeyPressed(const std::string& key)
{
    std::function<void()> fn = nullptr;

    if (key == "BACKSPACE")
    {
        if (!_input.empty()) { _input.pop_back(); }
    }
    else if (key == "ENTER")
    {
        std::string value = _input;

        _input.clear();

        fn = [value]()
        {
            Close();

            OnEnterPressed->Emit(value);
        };
    }
    else if (key == "SHIFT")
    {
        // TODO: Shift
        _capslockOn = !_capslockOn;
    }
    else if (key == "SPACE") { _input += ' '; }
    else
    {
        std::string inputKey = key;

        if (_capslockOn)
        {
            for (char& character : inputKey) { character = static_cast<char>(std::toupper(static_cast<unsigned char>(character))); }
        }

        _input += inputKey;
    }

    FormatInput();
    UpdateInputText();

    if (fn != nullptr) { fn(); }
}

void Keyboard::FormatInput()
{
    if ((static_cast<uint32_t>(_flags) & static_cast<uint32_t>(KeyboardFlags::NumbersOnly)) != 0)
    {
        for (auto it = _input.begin(); it != _input.end();)
        {
            if (!std::isdigit(static_cast<unsigned char>(*it))) { it = _input.erase(it); }
            else
            {
                ++it;
            }
        }
    }
}

void Keyboard::UpdateInputText()
{
    // Atualiza o texto exibido no teclado.
    auto* inputContainer = mainContainer->FindChild("inputContainer");

    if (inputContainer) { inputContainer->text = _input; }
}