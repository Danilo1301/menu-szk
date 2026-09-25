#include "input.h"

#include "aml-psdk/game_sa/Events.h"
#include "aml-psdk/game_sa/utils/ThreadSyncer.h"
#include "container/container.h"
#include "global_config.h"
#include "menuSZK.h"
#include "menus/menuDebugOptions.h"
#include "mod/logger.h"

#include "container/container.h"
#include "pch.h"
#include "utils/drawUtils.h"
#include "utils/eventListener.h"
#include <cstddef>

EventListener<int, SwipeDirection> *Input::OnSwipe = new EventListener<int, SwipeDirection>();
EventListener<int, std::string> *Input::OnSwipeSpecial = new EventListener<int, std::string>();
EventListener<int> *Input::OnTouchMove = new EventListener<int>();
EventListener<int> *Input::OnTouchUp = new EventListener<int>();

std::unordered_map<int, InputTouch> Input::_touches;
std::unordered_map<std::string, SwipeDefinition> Input::_swipeAreas;

void Input::Initialize()
{
    // logger->Info("touch event now is listening");

    // Events::touchScreenEvent += [](int actionType, int trackNum, int x, int y)
    // {
    //     logger->Info("touch event no event");

    //     ThreadSyncer::PushToMain([actionType, trackNum, x, y]()
    //     {
    //         logger->Info("touch event passou pro thread");

    //         Input::OnTouchEvent(actionType, trackNum, x, y);
    //     });
    // };
}

void Input::OnTouchEvent(int actionType, int trackNum, int x, int y, unsigned int time)
{
    auto screenResolution = DrawUtils::GetBaseResolution();

    CVector2D position(DrawUtils::MapWidthFromOS((float)x), DrawUtils::MapHeightFromOS((float)y));

    if (LOG_INPUTS)
        logger->Info("TouchEvent actionType=%d", actionType);

    switch (actionType)
    {
    case 2: // down
    {
        InputTouch &touch = _touches[trackNum];

        touch.active = true;
        touch.startPosition = position;
        touch.position = position;
        touch.lastPosition = position;
        touch.delta = CVector2D(0.0f, 0.0f);
        touch.startTime = time;

        break;
    }

    case 3: // move
    {
        auto it = _touches.find(trackNum);

        if (it == _touches.end())
            break;

        InputTouch &touch = it->second;

        touch.delta = position - touch.position;
        touch.lastPosition = touch.position;
        touch.position = position;

        OnTouchMove->Emit(trackNum);

        break;
    }

    case 1: // up
    {
        auto it = _touches.find(trackNum);

        if (it == _touches.end())
            break;

        InputTouch &touch = it->second;

        touch.delta = position - touch.position;
        touch.lastPosition = touch.position;
        touch.position = position;

        if (IsSwipeDown(touch, 300.0f, screenResolution.height - 300.0f, 400.0f))
        {
            OnSwipe->Emit(trackNum, SwipeDirection::Down);
        }

        if (IsSwipeUp(touch, screenResolution.height - 300.0f, 300.0f, 400.0f))
        {
            OnSwipe->Emit(trackNum, SwipeDirection::Up);
        }

        logger->Info("Verifying %d swipes definitions", _swipeAreas.size());

        for (const auto &[id, swipe] : _swipeAreas)
        {
            if (IsSwipe(touch, swipe))
            {
                OnSwipeSpecial->Emit(trackNum, swipe.customId);
            }
        }

        OnTouchUp->Emit(trackNum);

        _touches.erase(it);

        break;
    }
    default:
        break;
    }

    if (actionType == 2)
    {
        auto container = Container::MainContainer->GetContainerAtPosition(position, true);

        if (container != nullptr)
        {
            LOGI("Input: %d down on %s", trackNum, container->tag.c_str());

            container->HandleOnDown(trackNum);
        }
    }

    if (actionType == 1)
    {
        // auto container = Container::MainContainer->GetContainerAtPosition(position, true);

        // if(container != nullptr && container->onClick->GetListenersCount() > 0 && container->CanBeClicked())
        // {

        // }
    }
}

bool Input::NeedsToBeBlocked(int x, int y)
{
    CVector2D position(DrawUtils::MapWidthFromOS((float)x), DrawUtils::MapHeightFromOS((float)y));

    return Container::MainContainer->ContainsBlockedInput(position);
}

const InputTouch *Input::GetTouch(int trackNum)
{
    for (auto &touch : _touches)
    {
        if (touch.first == trackNum)
            return &touch.second;
    }

    return nullptr;
}

std::unordered_map<int, InputTouch> *Input::GetTouches() { return &_touches; }

bool Input::IsSwipe(const InputTouch &touch, SwipeDirection direction, float startMinY, float startMaxY, float endMinY,
    float endMaxY, float xThreshold)
{
    auto screenResolution = DrawUtils::GetBaseResolution();

    const float centerX = screenResolution.width / 2.0f;

    if (touch.startPosition.y < startMinY || touch.startPosition.y > startMaxY)
        return false;

    if (touch.position.y < endMinY || touch.position.y > endMaxY)
        return false;

    if (std::abs(touch.startPosition.x - centerX) > xThreshold)
        return false;

    if (std::abs(touch.position.x - centerX) > xThreshold)
        return false;

    if (direction == SwipeDirection::Down)
        return touch.position.y > touch.startPosition.y;

    if (direction == SwipeDirection::Up)
        return touch.position.y < touch.startPosition.y;

    return false;
}

bool Input::IsSwipeDown(const InputTouch &touch, float startMaxY, float endMinY, float xThreshold)
{
    auto screenResolution = DrawUtils::GetBaseResolution();

    return IsSwipe(touch, SwipeDirection::Down, 0.0f, startMaxY, endMinY, screenResolution.height, xThreshold);
}

bool Input::IsSwipeUp(const InputTouch &touch, float startMinY, float endMaxY, float xThreshold)
{
    auto screenResolution = DrawUtils::GetBaseResolution();

    return IsSwipe(touch, SwipeDirection::Up, startMinY, screenResolution.height, 0.0f, endMaxY, xThreshold);
}

bool Input::IsSwipe(const InputTouch &touch, const SwipeDefinition &swipe)
{
    logger->Info("SWIPE [%s] start=(%.1f, %.1f) current=(%.1f, %.1f)", swipe.customId.c_str(), touch.startPosition.x,
        touch.startPosition.y, touch.position.x, touch.position.y);

    logger->Info("SWIPE [%s] startArea pos=(%.1f, %.1f) size=(%.1f, %.1f)", swipe.customId.c_str(),
        swipe.start.position.x, swipe.start.position.y, swipe.start.size.x, swipe.start.size.y);

    logger->Info("SWIPE [%s] endArea pos=(%.1f, %.1f) size=(%.1f, %.1f)", swipe.customId.c_str(), swipe.end.position.x,
        swipe.end.position.y, swipe.end.size.x, swipe.end.size.y);

    const bool startInside = IsInsideSwipeArea(touch.startPosition, swipe.start);

    const bool endInside = IsInsideSwipeArea(touch.position, swipe.end);

    logger->Info("SWIPE [%s] startInside=%d endInside=%d", swipe.customId.c_str(), startInside, endInside);

    if (!startInside)
    {
        logger->Info("SWIPE [%s] FAILED: start outside", swipe.customId.c_str());
        return false;
    }

    if (!endInside)
    {
        logger->Info("SWIPE [%s] FAILED: end outside", swipe.customId.c_str());
        return false;
    }

    if (swipe.direction == SwipeDirection::Up)
    {
        if (touch.position.y >= touch.startPosition.y)
        {
            logger->Info("SWIPE [%s] FAILED: expected UP, startY=%.1f endY=%.1f", swipe.customId.c_str(),
                touch.startPosition.y, touch.position.y);

            return false;
        }
    }

    if (swipe.direction == SwipeDirection::Down)
    {
        if (touch.position.y <= touch.startPosition.y)
        {
            logger->Info("SWIPE [%s] FAILED: expected DOWN, startY=%.1f endY=%.1f", swipe.customId.c_str(),
                touch.startPosition.y, touch.position.y);

            return false;
        }
    }

    logger->Info("SWIPE [%s] SUCCESS", swipe.customId.c_str());

    return true;
}

bool Input::IsInsideSwipeArea(const CVector2D &point, const SwipeArea &area)
{
    return point.x >= area.position.x && point.x <= area.position.x + area.size.x && point.y >= area.position.y &&
        point.y <= area.position.y + area.size.y;
}

void Input::RegisterSwipeArea(const SwipeDefinition &def) { _swipeAreas[def.customId] = def; }

void Input::DrawSwipeAreas()
{
    // logger->Info("debugOptions = %p", debugOptions);

    bool *value = debugOptions->GetBool("draw_swipe_areas");

    // logger->Info("value = %p", value);

    bool canDraw = *value;

    if (!canDraw)
        return;

    for (const auto &[id, swipe] : _swipeAreas)
    {
        DrawUtils::DrawRect(swipe.start.position, swipe.start.size, CRGBA(0, 255, 0, 50));

        DrawUtils::DrawRect(swipe.end.position, swipe.end.size, CRGBA(255, 0, 0, 50));
    }
}