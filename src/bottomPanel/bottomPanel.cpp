#include "bottomPanel.h"
#include "../config.h"
#include "../container/container.h"
#include "../container/containerLoader.h"
#include "../input.h"
#include "../utils/drawUtils.h"
#include "../utils/effects.h"
#include "aml-psdk/gta_base/Vector.h"

BottomPanel *BottomPanel::Main = nullptr;

BottomPanel *BottomPanel::CreateMain()
{
    auto panel = new BottomPanel();
    Main = panel;

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
    _container->style.opacity = 0.0f;
    SetVisible(true);

    const IResolution baseResolution = DrawUtils::GetBaseResolution();

    const CVector2D startPosition(baseResolution.width / 2.0f, baseResolution.height);

    const CVector2D endPosition(baseResolution.width / 2.0f, baseResolution.height - 190.0f);

    const CVector2D startScale(0.0f, 0.0f);
    const CVector2D endScale(1.0f, 1.0f);

    const float startOpacity = 0.0f;
    const float endOpacity = 1.0f;

    const int duration = 500;

    const auto onComplete = [this]() { SetVisible(true); };

    Ease_Curve(
        _container, startPosition, endPosition, startScale, endScale, startOpacity, endOpacity, duration, onComplete);
}

void BottomPanel::FadeOut()
{
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

    const auto onComplete = [this]() { SetVisible(false); };

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

    // Force update
    item->UpdateTransformFromRoot();

    const CVector2D itemSize = item->GetCurrentSize();
    const float itemWidth = itemSize.x;

    const int index = static_cast<int>(_items.size()) - 1;

    item->style.left = std::to_string(index * itemWidth) + "px";

    item->canBlockTouchEvents = true;
    item->style.backgroundImage = imagePath;

    _container->style.width = std::to_string(_items.size() * itemWidth) + "px";

    item->onClick->Add(
        [this, callback]()
        {
            FadeOut();
            callback();
        });
}

bool BottomPanel::IsVisible() { return _container->visible; }