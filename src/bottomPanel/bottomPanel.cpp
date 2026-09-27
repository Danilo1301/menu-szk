#include "bottomPanel.h"
#include "../config.h"
#include "../container/container.h"
#include "../container/containerLoader.h"
#include "../input.h"
#include "../utils/drawUtils.h"
#include "../utils/effects.h"
#include "aml-psdk/gta_base/Vector.h"

#include "../audio/audioUtils.h"
#include "src/pch.h"

BottomPanel *BottomPanel::Main = nullptr;

BottomPanel *BottomPanel::CreateMain()
{
    auto panel = new BottomPanel();
    Main = panel;

    panel->SetVisible(false);

    //

    {
        SwipeDefinition swipe;

        swipe.customId = "bottom_panel_swipe_up";
        swipe.direction = SwipeDirection::Up;

        swipe.start.position = CVector2D(900, 780);
        swipe.start.size = CVector2D(600, 300);

        swipe.end.position = CVector2D(900, 0);
        swipe.end.size = CVector2D(600, 780);

        Input::RegisterSwipeArea(swipe);
    }

    {
        SwipeDefinition swipe;

        swipe.customId = "bottom_panel_swipe_down";
        swipe.direction = SwipeDirection::Down;

        swipe.start.position = CVector2D(900, 0);
        swipe.start.size = CVector2D(600, 780);

        swipe.end.position = CVector2D(900, 780);
        swipe.end.size = CVector2D(600, 300);

        Input::RegisterSwipeArea(swipe);
    }

    //

    Input::OnSwipeSpecial->Add(
        [](int trackNum, std::string id)
        {
            if (id == "bottom_panel_swipe_down")
            {
                if (BottomPanel::Main->IsVisible())
                {
                    BottomPanel::Main->FadeOut();
                }
            }
            if (id == "bottom_panel_swipe_up")
            {
                BottomPanel::Main->FadeIn();
            }
        });

    Input::OnTouchUp->Add(
        [](int trackId)
        {
            if (BottomPanel::Main->IsVisible() && BottomPanel::Main->GetTimeVisible() > 500)
            {
                if (!BottomPanel::Main->IsPointerInside(trackId))
                {
                    BottomPanel::Main->FadeOut();
                }
            }
        });

    //

    return Main;
}

BottomPanel::BottomPanel()
{
    auto container = Container::MainContainer->AddChild("bottomPanel");
    LoadContainerFromFile(container, GetMenuLayoutPath("bottom_panel.json"));

    // container->style.left = "0px";
    // container->style.top = "0px";
    // container->style.right = "auto";
    // container->style.bottom = "auto";

    // container->onUpdateTransform->Add([container]() { container->Dump(); });

    _container = container;
}

void BottomPanel::SetVisible(bool visible) { _container->visible = visible; }

void BottomPanel::FadeIn()
{
    if (_isFading)
        return;

    _isFading = true;

    _timeOpened = g_timeInMilliseconds;

    _container->style.opacity = 0.0f;
    _container->style.scale = CVector2D(0, 0);
    SetVisible(true);

    const IResolution baseResolution = DrawUtils::GetBaseResolution();

    const CVector2D startPosition(baseResolution.width / 2.0f, baseResolution.height);

    const CVector2D endPosition(baseResolution.width / 2.0f, baseResolution.height - 190.0f);

    const CVector2D startScale(0.0f, 0.0f);
    const CVector2D endScale(1.0f, 1.0f);

    const float startOpacity = 0.0f;
    const float endOpacity = 1.0f;

    const int duration = 500;

    const auto onComplete = [this]()
    {
        SetVisible(true);
        _isFading = false;
    };

    Ease_Curve(
        _container, startPosition, endPosition, startScale, endScale, startOpacity, endOpacity, duration, onComplete);
}

void BottomPanel::FadeOut()
{
    if (_isFading)
        return;
    _isFading = true;

    _container->style.opacity = 1.0f;
    SetVisible(true);

    const IResolution baseResolution = DrawUtils::GetBaseResolution();

    const CVector2D startPosition(baseResolution.width / 2.0f, baseResolution.height - 190.0f);

    const CVector2D endPosition(baseResolution.width / 2.0f, baseResolution.height);

    const CVector2D startScale(1.0f, 1.0f);
    const CVector2D endScale(0.0f, 0.0f);

    const float startOpacity = 1.0f;
    const float endOpacity = 0.0f;

    const int duration = 500;

    const auto onComplete = [this]()
    {
        SetVisible(false);
        _isFading = false;
    };

    Ease_Simple(
        _container, startPosition, endPosition, startScale, endScale, startOpacity, endOpacity, duration, onComplete);
}

void BottomPanel::AddItem(std::string title, std::string imagePath, std::function<void()> callback)
{
    if (imagePath.empty())
    {
        imagePath = GetMenuAssetPath("icons/script.png");
    }

    _items.push_back({title, imagePath, callback});

    auto item = _container->AddChild("item");
    LoadContainerFromFile(item, GetMenuLayoutPath("bottom_panel_item.json"));

    auto text = item->FindChild("text");
    text->text = title;

    item->canBlockTouchEvents = true;
    item->style.backgroundImage = imagePath;

    item->onClick->Add(
        [this, callback]()
        {
            FadeOut();
            callback();

            PlaySelect();
        });

    _itemContainers.push_back(item);

    // workaround, should have its own function i think
    _container->visible = true;
    _container->UpdateTransform();
    _container->visible = false;

    const float itemWidth = _itemContainers[0]->GetCurrentSize().x;

    for (size_t i = 0; i < _itemContainers.size(); i++)
    {
        auto item = _itemContainers[i];

        item->style.left = std::to_string(i * itemWidth) + "px";
        item->style.right = "auto";

        item->UpdateTransform();
    }

    _container->style.width = std::to_string(_itemContainers.size() * itemWidth) + "px";
}

void BottomPanel::UpdateItemsLayout() {}

bool BottomPanel::IsVisible() { return _container->visible; }

bool BottomPanel::IsPointerInside(int trackId)
{
    auto touch = Input::GetTouch(trackId);

    if (!touch)
        return false;

    return _container->IsPositionInside(touch->position);
}

int BottomPanel::GetTimeVisible()
{
    auto now = g_timeInMilliseconds;

    return now - _timeOpened;
}