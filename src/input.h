#pragma once

#include "pch.h"

#include "aml-psdk/gta_base/Vector.h"
#include "utils/eventListener.h"

#include <string>
#include <sys/stat.h>
#include <unordered_map>

struct InputTouch
{
    bool active = false;

    CVector2D startPosition;
    CVector2D position;
    CVector2D lastPosition;
    CVector2D delta;

    float startTime = 0.0f;
};

enum class SwipeDirection
{
    Up,
    Down,
    Left,
    Right
};

struct SwipeArea
{
    CVector2D position;
    CVector2D size;
};

struct SwipeDefinition
{
    std::string customId = "";
    SwipeArea start;
    SwipeArea end;
    SwipeDirection direction;
};

class Input
{
  public:
    static EventListener<int, SwipeDirection> *OnSwipe;
    static EventListener<int, std::string> *OnSwipeSpecial;
    static EventListener<int> *OnTouchMove;
    static EventListener<int> *OnTouchUp;

    static void Initialize();

    static void OnTouchEvent(int actionType, int trackNum, int x, int y, unsigned int time);

    static bool NeedsToBeBlocked(int x, int y);

    static const InputTouch *GetTouch(int trackNum);
    static std::unordered_map<int, InputTouch> *GetTouches();

    static bool IsSwipe(const InputTouch &touch, SwipeDirection direction, float startMinY, float startMaxY,
        float endMinY, float endMaxY, float xThreshold);

    static bool IsSwipeDown(const InputTouch &touch, float startMaxY, float endMinY, float xThreshold);
    static bool IsSwipeUp(const InputTouch &touch, float startMinY, float endMaxY, float xThreshold);

    static bool IsSwipe(const InputTouch &touch, const SwipeDefinition &swipe);
    static bool IsInsideSwipeArea(const CVector2D &point, const SwipeArea &area);

    static void RegisterSwipeArea(const SwipeDefinition &def);

    static void DrawSwipeAreas();

  private:
    static std::unordered_map<int, InputTouch> _touches;
    static std::unordered_map<std::string, SwipeDefinition> _swipeAreas;
};