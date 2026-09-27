#pragma once

#include "aml-psdk/game_sa/entity/Placeable.h"
#include "aml-psdk/game_sa/plugin.h"
#include "aml-psdk/gta_base/RGBA.h"
#include "aml-psdk/gta_base/Vector.h"
#include "aml-psdk/renderware/RwTexture.h"

#include <functional>
#include <string>
#include <vector>

struct GameEntity
{
    void *ptr;
    int ref;
};

template <typename... Args> class IEventListener
{
  public:
    using Callback = std::function<void(Args...)>;
    using ConditionCallback = std::function<bool(Args...)>;

    virtual ~IEventListener() = default;

    virtual void Add(const Callback &cb) = 0;
    virtual void AddRef(void *ref, const Callback &cb) = 0;
    virtual void AddOnce(const Callback &cb) = 0;
    virtual void AddUntil(const ConditionCallback &cb) = 0;
    virtual void Emit(Args... args) = 0;
    virtual void Remove(void *ref) = 0;

    virtual int GetListenersCount() = 0;
};

enum MenuFontAlignment : unsigned char
{
    ALIGN_CENTER,
    ALIGN_LEFT,
    ALIGN_RIGHT
};

enum MenuFontStyle : unsigned char
{
    FONT_GOTHIC,
    FONT_SUBTITLES,
    FONT_MENU,
    FONT_PRICEDOWN
};

struct IFont
{
  public:
    float size = 1.0f;
    MenuFontAlignment align = MenuFontAlignment::ALIGN_LEFT;
    MenuFontStyle style = MenuFontStyle::FONT_SUBTITLES;
    CRGBA color = CRGBA(255, 255, 255);
    CRGBA clickedColor = CRGBA(0, 0, 0);
};

struct MenuMargin
{
    float top = 0;
    float left = 0;
    float bottom = 0;
    float right = 0;
};

enum class HorizontalAlign
{
    Left = 0,
    Middle,
    Right
};

enum class VerticalAlign
{
    Top = 0,
    Middle,
    Bottom
};

enum IContainerState
{
    Normal,
    Clicked,
    Disabled
};

class IStyle
{
  public:
    std::string position = "absolute";

    std::string left = "0px";
    std::string right = "auto";
    std::string top = "0px";
    std::string bottom = "auto";

    std::string width = "200px";
    std::string height = "200px";

    CVector2D scale = CVector2D(1, 1);
    float opacity = 1.0f;

    MenuMargin margin = MenuMargin();

    CRGBA backgroundColor = CRGBA(255, 255, 255, 0);
    CRGBA backgroundColorClicked = CRGBA(255, 255, 255, 0);
    CRGBA imageColor = CRGBA(255, 255, 255, 255);

    std::string backgroundImage = "";

    CVector2D transformOrigin = CVector2D(-1.0f, -1.0f);

    HorizontalAlign textHorizontalAlign = HorizontalAlign::Left;
    VerticalAlign textVerticalAlign = VerticalAlign::Middle;

    CVector2D textOffset = CVector2D(0, 0);
};

enum IBlockType
{
    None,
    BlockThisAndChildren,
    BlockInheritedFromParent
};

class IContainer
{
  public:
    std::string tag;
    std::string text;

    bool visible = true;
    IBlockType block = IBlockType::None;

    IFont textFont;

    IStyle style;

    bool canBlockTouchEvents = false;
    bool canClickThrough = false;
    bool canDrag = false;
    bool drawBoundings = false;
    bool hideWhenPaused = false;

    IEventListener<> *onClick;
    IEventListener<IContainerState> *onStateChanged;

    IEventListener<> *onPreUpdateTransform;
    IEventListener<> *onPostUpdateTransform;

    IContainerState state = IContainerState::Normal;

    virtual IContainer *AddChild_I(std::string tag) = 0;
    virtual IContainer *FindChild_I(std::string tag) = 0;
    virtual void SetDisabled(bool disabled) = 0;
    virtual void Destroy() = 0;

    void SetRelativePosition(float x, float y)
    {
        style.left = std::to_string(x) + "px";
        style.right = "auto";
        style.top = std::to_string(y) + "px";
        style.bottom = "auto";
    }
};

class IMenuItem
{
  public:
    IEventListener<> *onValueChange;

    virtual bool GetBoolValue() = 0;

    virtual void AddOption(int value, std::string displayText) = 0;
    virtual int GetCurrentOptionValue() = 0;

    virtual IContainer *GetContainer() = 0;

    virtual void AddColorPreview(CRGBA *color) = 0;

    virtual void AddIcon(std::string pngFilePath) = 0;
};

class IWindow
{
  public:
    std::string title = "Titulo";
    std::string subTitle = "";
    float width = 800;
    CRGBA windowColor = CRGBA(255, 0, 0);
    bool blocked = false;
    int maxItemsPerPage = 5;

    virtual IMenuItem *AddCheckbox(std::string text, bool *pValue) = 0;
    virtual IMenuItem *AddFloatOptions(std::string text, float *pValue, float min, float max, float step) = 0;
    virtual IMenuItem *AddIntOptions(std::string text, int *pValue, int min, int max, int step) = 0;
    virtual IMenuItem *AddOptions(std::string text, float optionsWidth = 450.0f) = 0;
    virtual IMenuItem *AddItem(std::string text) = 0;
    virtual IMenuItem *AddButton(std::string text, std::function<void()> onClick) = 0;
    virtual IMenuItem *AddSlider(std::string text, float *pValue, float minValue, float maxValue, int decimals) = 0;
    virtual IMenuItem *AddCustomItem(std::string text, float height) = 0;

    virtual void Close() = 0;

    virtual IWindow *OpenColorMenu(CRGBA *color) = 0;

    IEventListener<> *onClose;
};

class IWidget
{
  public:
    IEventListener<> *onClick = 0;
    bool visible = true;

    virtual void SetPosition(float x, float y) = 0;
    virtual void SetSize(float size) = 0;
};

struct IAudio
{
  public:
    virtual ~IAudio() = default;

    virtual void Play() = 0;
    virtual void Stop() = 0;
    virtual void SetLoop(bool loop) = 0;
    virtual bool Finished() = 0;
    virtual bool Loaded() = 0;
    virtual bool Is3D() = 0;
    virtual void AttachToCPlaceable(CPlaceable *ptr) = 0;
    virtual void SetVolume(float volume) = 0;
};

class IMenuSZK
{
  public:
    virtual IWindow *CreateWindow(float x, float y, float width, std::string title, std::string subtitle) = 0;

    virtual void AddCellphoneScript(std::string text, std::string iconPath, std::function<void()> fn) = 0;

    IEventListener<GameEntity> *onPedAdded = 0;
    IEventListener<GameEntity> *onPedRemoved = 0;

    IEventListener<GameEntity> *onVehicleAdded = 0;
    IEventListener<GameEntity> *onVehicleRemoved = 0;

    IEventListener<> *onPlayerReady = 0;

    IEventListener<unsigned int> *onGameProcess = 0;
    IEventListener<unsigned int> *onPreRenderEnd = 0;

    // i think its safe to can call opcodes here
    IEventListener<unsigned int> *onScriptProcess = 0;

    // NOT IMPLEMENTED
    IEventListener<> *onPostDrawRadar = 0;

    virtual std::vector<GameEntity> GetPeds() = 0;
    virtual std::vector<GameEntity> GetVehicles() = 0;

    virtual IWidget *CreateWidget(float x, float y, float size, std::string bgImage, std::string image) = 0;

    virtual RwTexture *LoadTexture(std::string pngFilePath, bool cache = true) = 0;

    virtual IContainer *GetMainContainer() = 0;
};

// parei aqui no virtual IContainer *GetMainContainer() = 0;
// falta o create widget