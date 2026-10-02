#pragma once

#include "../pch.h"
#include "../utils/textureLoader.h"

#include "aml-psdk/gta_base/Vector.h"
#include "cssValue.h"
#include "menuSZK/imenuSZK.h"
#include <vector>

struct RwTexture;
class Texture;

class Container : public IContainer
{
private:
    CSSValue cssLeft;
    CSSValue cssRight;
    CSSValue cssTop;
    CSSValue cssBottom;
    CSSValue cssWidth;
    CSSValue cssHeight;

    CVector2D currentPosition = CVector2D(0, 0);
    CVector2D currentSize = CVector2D(0, 0);
    CVector2D currentScale = CVector2D(1, 1);
    float currentOpacity = 1.0f;

    int _touchTrackId = -1;
    bool _isDragging = false;
    CVector2D _dragStartPosition;
    CVector2D _dragStartTouchPosition;
    CVector2D _touchStartPosition;

    Texture* backgroundTexture = nullptr;

    Container* parent = nullptr;
    std::vector<Container*> children;

    unsigned int timeClicked = 0;
    unsigned int timeCreated = 0;

    uint64_t _instanceId;
    std::string _prevBackgroundImage = "";

    bool _clickedState = false;
    bool _destroyed = false;

    int _priority = 0;

public:
    Container(std::string tag);
    ~Container();

    Container* AddChild(Container* child);
    Container* AddChild(std::string tag);
    IContainer* AddChild_I(const std::string& tag) override
    {
        return (IContainer*)AddChild(tag);
    }

    void SetParent(Container* parent);
    void RemoveChild(Container* child, bool destroy);
    void RemoveChildren();

    void SortChildren();

    Container* FindChild(std::string tag);
    IContainer* FindChild_I(const std::string& tag) override
    {
        return (IContainer*)FindChild(tag);
    }

    std::vector<Container*> GetChildrenRecursive();

    void Update();
    void UpdateTransform();
    void Draw();

    void UpdateTransformFromRoot();

    bool IsVisible();

    int GetPriority();

private:
    void DrawBoundings();

    void ResolveBackgroundImages();

public:
    CVector2D GetCenterPosition();
    CVector2D GetRelativePosition();
    CVector2D GetCurrentSize();
    CVector2D LocalToOther(const CVector2D& position, Container* other);

    bool ContainsBlockedInput(const CVector2D& position);
    bool IsPositionInside(const CVector2D& position);
    Container* GetContainerAtPosition(const CVector2D& position, bool mustBeClickable = false);

    void HandleOnDown(int trackId);
    void HandleOnMove(int trackId);
    void HandleOnUp(int trackId);
    void HandleOnClick();

    bool CanBeClicked();

    void SetBackgroundImageIgnoreStyle(std::string bgFilePath);
    void SetBackgroundTexture(Texture* texture);

    void Dump();

    void SetState(IContainerState state);
    void SetDisabled(bool disabled) override;
    void SetPriority(int priority) override;

    void SetBlocked(bool blocked);

    void Destroy() override;

    void MarkAsDestroyed();

    uint64_t GetInstanceId() const
    {
        return _instanceId;
    }

    int GetCurrentTrackingTouchId()
    {
        return _touchTrackId;
    }

public:
    static bool ShowTag;
    static Container* MainContainer;
    static Container* CreateContainer(std::string tag);
    static void DestroyContainersThatNeedsToBeDestroyed();
    static void SortContainersThatNeedsToBeSorted();
};