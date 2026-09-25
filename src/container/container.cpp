#include "container.h"

#include "../menuSZK.h"
#include "../utils/drawUtils.h"
#include "../utils/eventListener.h"

#include "aml-psdk/gta_base/RGBA.h"
#include "aml-psdk/gta_base/Vector.h"

#include "menu/menu.h"
#include "mod/logger.h"

#include <cstddef>
#include <string>
#include <sys/stat.h>

#include "../input.h"

#include "../menuInterface.h"

#include "../menus/menuDebugOptions.h"

bool Container::ShowTag = true;
Container *Container::MainContainer = nullptr;

Container::Container(std::string tag)
{
    static uint64_t _lastInstanceId = 1;
    _instanceId = ++_lastInstanceId;

    LeakUtils::RegisterItem("Container");

    this->tag = tag;

    this->onClick = new EventListener<>();
    this->onStateChanged = new EventListener<IContainerState>();

    this->onPreUpdateTransform = new EventListener();
    this->onPostUpdateTransform = new EventListener();

    this->timeCreated = g_timeInMilliseconds;
}

Container::~Container()
{
    LOGI("Destroying container %s", tag.c_str());
    LOG_PER_FRAME("Destroying container " + std::to_string(reinterpret_cast<uintptr_t>(this)));

    LeakUtils::FreeItem("Container");

    if (parent != nullptr)
    {
        LOGI("Removing from parent %s", parent->tag.c_str());
        parent->RemoveChild(this, false);
    }

    LOGI("Destroyig textures");

    if (backgroundTexture != nullptr)
        delete backgroundTexture;

    LOGI("Removing childrean");

    RemoveChildren();
}

Container *Container::AddChild(Container *child)
{
    if (child == this)
    {
        logger->Error("Container.AddChild(%s) You cant add himself as his child", child->tag.c_str());
        std::abort();
    }

    children.push_back(child);

    child->SetParent(this);

    return child;
}

Container *Container::AddChild(std::string tag)
{
    auto container = Container::CreateContainer(tag);

    AddChild(container);

    return container;
}

void Container::SetParent(Container *parent)
{
    if (this->parent != nullptr)
    {
        logger->Error(
            "Container.Draw(%s) This container has already a parent. Cant set parent to ", parent->tag.c_str());
        std::abort();
    }

    if (!parent)
    {
        logger->Error("Parent is null during Container.SetParent");
    }

    this->parent = parent;
}

void Container::RemoveChild(Container *child, bool destroy)
{
    auto it = std::find(children.begin(), children.end(), child);

    if (it == children.end())
        return;

    Container *removedChild = *it;

    removedChild->parent = nullptr;

    children.erase(it);

    if (destroy)
        delete removedChild;
}

void Container::RemoveChildren()
{
    auto childrenCopy = children;

    for (auto child : childrenCopy)
    {
        RemoveChild(child, true);
    }
}

Container *Container::FindChild(std::string tag)
{
    for (auto child : children)
    {
        if (!child)
        {
            logger->Error("FindChild(%s) - NULL child!", tag.c_str());
            continue;
        }

        if (child->tag == tag)
        {
            return child;
        }

        auto result = child->FindChild(tag);

        if (result)
            return result;
    }

    logger->Info("Could not find child with name %s", tag.c_str());

    return nullptr;
}

std::vector<Container *> Container::GetChildrenRecursive()
{
    std::vector<Container *> result;

    for (auto child : children)
    {
        result.push_back(child);

        auto recursiveChildren = child->GetChildrenRecursive();

        result.insert(result.end(), recursiveChildren.begin(), recursiveChildren.end());
    }

    return result;
}

void Container::Draw()
{
    if (!visible)
        return;

    unsigned int now = g_timeInMilliseconds;

    bool clickedVisuals = state == IContainerState::Clicked;

    auto position = this->currentPosition;
    auto size = this->currentSize;

    auto bgColor = clickedVisuals ? style.backgroundColorClicked : style.backgroundColor;
    bgColor.a = (unsigned char)std::clamp(bgColor.a * currentOpacity, 0.0f, 255.0f);

    if (bgColor.a > 0)
    {
        DrawUtils::DrawRect(currentPosition, currentSize, bgColor);
    }

    if (backgroundTexture != nullptr)
    {
        auto texture = backgroundTexture;

        auto spriteColor = style.imageColor;

        spriteColor.a = (unsigned char)((float)spriteColor.a * currentOpacity);

        DrawUtils::DrawSprite(&texture->sprite, currentPosition, currentSize, spriteColor);
    }

    if (text.length() > 0)
    {
        auto textPosition = position;

        switch (style.textHorizontalAlign)
        {
        case HorizontalAlign::Left:
            textPosition.x = position.x;
            break;

        case HorizontalAlign::Middle:
            textPosition.x = position.x + size.x / 2.0f;
            break;

        case HorizontalAlign::Right:
            textPosition.x = position.x + size.x;
            break;
        }

        switch (style.textVerticalAlign)
        {
        case VerticalAlign::Top:
            textPosition.y = position.y;
            break;

        case VerticalAlign::Middle:
            textPosition.y = position.y + size.y / 2.0f;
            break;

        case VerticalAlign::Bottom:
            textPosition.y = position.y + size.y;
            break;
        }

        textPosition.x += style.textOffset.x;
        textPosition.y += style.textOffset.y;

        auto textScale = CVector2D(currentScale.x, currentScale.y);

        DrawUtils::DrawText(text, textPosition, textFont, textScale, clickedVisuals, currentOpacity);
    }

    if (drawBoundings || *debugOptions->GetBool("draw_container_boundings"))
    {
        DrawBoundings();
    }

    for (Container *child : children)
    {
        if (child == this)
        {
            logger->Error("Container.Draw(%s) This container has himself as his child", child->tag.c_str());
            std::abort();
        }

        child->Draw();
    }
}

void Container::UpdateTransformFromRoot()
{
    Container *root = this;

    while (root->parent != nullptr)
    {
        root = root->parent;
    }

    root->UpdateTransform();
}

void Container::DrawBoundings()
{
    bool clickedVisuals = state == IContainerState::Clicked || _touchTrackId != -1;

    CRGBA normalColor = CanBeClicked() ? CRGBA(255, 100, 0, 255) : CRGBA(255, 255, 0, 255);

    CRGBA color = clickedVisuals ? CRGBA(130, 0, 255) : normalColor;

    IFont font;
    font.size = 2.0f;
    font.align = MenuFontAlignment::ALIGN_LEFT;
    font.color = color;

    // auto textPosition = currentPosition + (currentSize / 2);
    auto textPosition = currentPosition;

    DrawUtils::DrawText(tag, textPosition, font, CVector2D(1, 1), false, currentOpacity);

    const float thickness = 2.0f;

    // Top
    DrawUtils::DrawRect(currentPosition, CVector2D(currentSize.x, thickness), color);

    // Bottom
    DrawUtils::DrawRect(CVector2D(currentPosition.x, currentPosition.y + currentSize.y - thickness),
        CVector2D(currentSize.x, thickness), color);

    // Left
    DrawUtils::DrawRect(currentPosition, CVector2D(thickness, currentSize.y), color);

    // Right
    DrawUtils::DrawRect(CVector2D(currentPosition.x + currentSize.x - thickness, currentPosition.y),
        CVector2D(thickness, currentSize.y), color);
}

void Container::ResolveBackgroundImages()
{
    if (_prevBackgroundImage != style.backgroundImage)
    {
        _prevBackgroundImage = style.backgroundImage;

        logger->Info("Detected background image change: %s", style.backgroundImage.c_str());

        SetBackgroundImageIgnoreStyle(style.backgroundImage);
    }
}

void Container::UpdateTransform()
{
    // LOG_PER_FRAME("Updating " + tag);

    unsigned int now = g_timeInMilliseconds;

    if (state == IContainerState::Clicked)
    {
        if (now - timeClicked >= 180)
        {
            SetState(IContainerState::Normal);
        }
    }

    if (!visible)
        return;

    ResolveBackgroundImages();

    if (onPreUpdateTransform)
    {
        onPreUpdateTransform->Emit();
    }

    IResolution baseResolution = DrawUtils::GetBaseResolution();

    if (parent == nullptr)
    {
        currentPosition = CVector2D(0.0f, 0.0f);

        currentSize = CVector2D(baseResolution.width, baseResolution.height);

        currentScale = CVector2D(style.scale.x, style.scale.y);

        currentOpacity = style.opacity;
    }
    else
    {
        Container *parentContainer = parent;

        const CVector2D parentSize = parentContainer->currentSize;

        const CVector2D parentPosition = parentContainer->currentPosition;

        const CVector2D parentScale = parentContainer->currentScale;

        const float parentOpacity = parentContainer->currentOpacity;

        // Tamanho lógico do parent, antes da escala.
        const CVector2D parentLogicalSize(parentSize.x / parentScale.x, parentSize.y / parentScale.y);

        float width = cssWidth.Parse(style.width, parentLogicalSize.x);

        float height = cssHeight.Parse(style.height, parentLogicalSize.y);

        const float left = cssLeft.Parse(style.left, parentLogicalSize.x);

        const float right = cssRight.Parse(style.right, parentLogicalSize.x);

        const float top = cssTop.Parse(style.top, parentLogicalSize.y);

        const float bottom = cssBottom.Parse(style.bottom, parentLogicalSize.y);

        const float marginLeft = style.margin.left;

        const float marginRight = style.margin.right;

        const float marginTop = style.margin.top;

        const float marginBottom = style.margin.bottom;

        // width: auto + left/right define o tamanho.
        if (style.position == "absolute" && style.width == "auto" && style.left != "auto" && style.right != "auto")
        {
            width = parentLogicalSize.x - left - right;
        }

        // height: auto + top/bottom define o tamanho.
        if (style.position == "absolute" && style.height == "auto" && style.top != "auto" && style.bottom != "auto")
        {
            height = parentLogicalSize.y - top - bottom;
        }

        // Margin ocupa espaço externo.
        if ((!style.width.empty() && style.width.back() == '%') || style.width == "auto")
        {
            width -= marginLeft + marginRight;
        }

        if ((!style.height.empty() && style.height.back() == '%') || style.height == "auto")
        {
            height -= marginTop + marginBottom;
        }

        width = std::max(0.0f, width);
        height = std::max(0.0f, height);

        currentScale = CVector2D(parentScale.x * style.scale.x, parentScale.y * style.scale.y);

        currentOpacity = parentOpacity * style.opacity;

        currentSize = CVector2D(width * currentScale.x, height * currentScale.y);

        if (block != IBlockType::None)
        {
            currentOpacity = 0.4;
        }

        float x = 0.0f;
        float y = 0.0f;

        if (style.position == "absolute")
        {
            if (style.right != "auto")
            {
                x = parentLogicalSize.x - width - right - marginRight;
            }
            else
            {
                x = left + marginLeft;
            }

            if (style.bottom != "auto")
            {
                y = parentLogicalSize.y - height - bottom - marginBottom;
            }
            else
            {
                y = top + marginTop;
            }
        }
        else
        {
            x = left + marginLeft;

            y = top + marginTop;
        }

        x *= parentScale.x;
        y *= parentScale.y;

        const CVector2D pivotOffset(((style.transformOrigin.x + 1.0f) / 2.0f) * currentSize.x,

            ((style.transformOrigin.y + 1.0f) / 2.0f) * currentSize.y);

        currentPosition = CVector2D(parentPosition.x + x - pivotOffset.x,

            parentPosition.y + y - pivotOffset.y);
    }

    if (onPostUpdateTransform)
    {
        onPostUpdateTransform->Emit();
    }

    // LOG_PER_FRAME("Updating children of " + tag);

    for (Container *child : children)
    {
        if (child == this)
        {
            logger->Error("Container.Draw(%s) This container has himself as his child", child->tag.c_str());

            std::abort();
        }

        // LOG_PER_FRAME("Updating child " + std::to_string(reinterpret_cast<uintptr_t>(this)));

        child->UpdateTransform();
    }
}

CVector2D Container::GetCenterPosition()
{
    return CVector2D(currentPosition.x + currentSize.x / 2.0f, currentPosition.y + currentSize.y / 2.0f);
}

CVector2D Container::GetRelativePosition()
{
    if (parent == nullptr)
        return CVector2D(0.0f, 0.0f);

    const CVector2D parentLogicalSize(
        parent->currentSize.x / parent->currentScale.x, parent->currentSize.y / parent->currentScale.y);

    const float width = cssWidth.Parse(style.width, parentLogicalSize.x);
    const float height = cssHeight.Parse(style.height, parentLogicalSize.y);

    float x;
    float y;

    if (style.right != "auto")
    {
        const float right = cssRight.Parse(style.right, parentLogicalSize.x);
        x = parentLogicalSize.x - width - right;
    }
    else
    {
        x = cssLeft.Parse(style.left, parentLogicalSize.x);
    }

    if (style.bottom != "auto")
    {
        const float bottom = cssBottom.Parse(style.bottom, parentLogicalSize.y);
        y = parentLogicalSize.y - height - bottom;
    }
    else
    {
        y = cssTop.Parse(style.top, parentLogicalSize.y);
    }

    return CVector2D(x, y);
}

CVector2D Container::GetCurrentSize() { return currentSize; }

CVector2D Container::LocalToOther(const CVector2D &position, Container *other)
{
    CVector2D worldPosition(
        currentPosition.x + position.x * currentScale.x, currentPosition.y + position.y * currentScale.y);

    return CVector2D((worldPosition.x - other->currentPosition.x) / other->currentScale.x,
        (worldPosition.y - other->currentPosition.y) / other->currentScale.y);
}

bool Container::ContainsBlockedInput(const CVector2D &position)
{
    bool inside = position.x >= currentPosition.x && position.x <= currentPosition.x + currentSize.x &&
        position.y >= currentPosition.y && position.y <= currentPosition.y + currentSize.y;

    if (!visible)
        return false;

    if (canBlockTouchEvents && inside)
    {
        // logger->Info("Container %s blocked input", tag.c_str());
        return true;
    }

    for (Container *child : children)
    {
        if (child->ContainsBlockedInput(position))
        {
            return true;
        }
    }

    return false;
}

bool Container::IsPositionInside(const CVector2D &position)
{
    bool inside = position.x >= currentPosition.x && position.x <= currentPosition.x + currentSize.x &&
        position.y >= currentPosition.y && position.y <= currentPosition.y + currentSize.y;

    return inside;
}

Container *Container::GetContainerAtPosition(const CVector2D &position, bool mustBeClickable)
{
    if (!visible)
        return nullptr;

    for (auto it = children.rbegin(); it != children.rend(); ++it)
    {
        Container *child = *it;

        if (Container *container = child->GetContainerAtPosition(position))
            return container;
    }

    bool inside = IsPositionInside(position);

    if (inside)
    {
        if (block != IBlockType::None)
        {
            return nullptr;
        }

        if (canClickThrough == false && CanBeClicked())
        {
            return this;
        }
    }

    return nullptr;
}

void Container::HandleOnDown(int trackId)
{
    logger->Info("HandleOnDown on container %s", tag.c_str());

    if (_touchTrackId == -1)
    {
        auto inputTouch = Input::GetTouch(trackId);

        if (!inputTouch)
        {
            logger->Error("Attempted to get touch but its null?");
        }

        _touchTrackId = trackId;
        _isDragging = false;
        _touchStartPosition = inputTouch->position;

        Input::OnTouchMove->AddRef(this, [this](int trackId) { HandleOnMove(trackId); });

        Input::OnTouchUp->AddRef(this, [this](int trackId) { HandleOnUp(trackId); });
    }
}

void Container::HandleOnUp(int trackId)
{
    auto inputTouch = Input::GetTouch(trackId);

    LOGI("Input: %d up on %s", trackId, tag.c_str());

    Input::OnTouchMove->Remove(this);
    Input::OnTouchUp->Remove(this);

    bool isTouchInside = IsPositionInside(inputTouch->position);

    if (!_isDragging && isTouchInside)
    {
        HandleOnClick();
    }

    if (_isDragging)
    {
        logger->Info("stopped dragging");
    }

    _touchTrackId = -1;
    _isDragging = false;
}

void Container::HandleOnMove(int trackId)
{
    auto inputTouch = Input::GetTouch(trackId);

    if (!inputTouch)
    {
        logger->Error("Moved and no inputTouch?");
    }

    if (canDrag)
    {
        if (!_isDragging)
        {
            float distance = _touchStartPosition.Distance(inputTouch->position);

            logger->Info("checking if we can drag");

            if (distance > 10.0f)
            {
                _isDragging = true;
                _dragStartPosition = GetRelativePosition();
                _dragStartTouchPosition = inputTouch->position;
            }
        }

        if (_isDragging)
        {
            auto pos = _dragStartPosition;

            pos.x += inputTouch->position.x - _dragStartTouchPosition.x;
            pos.y += inputTouch->position.y - _dragStartTouchPosition.y;

            SetRelativePosition(pos.x, pos.y);
        }
    }
    else
    {
        logger->Info("cant drag this one");
    }
}

void Container::HandleOnClick()
{
    logger->Info("Handling on click on this container, so we add ONCE to gameprocess");

    menuInterface->onGameProcess->AddOnce(
        [this](unsigned int deltaTime)
        {
            logger->Info("the once function got called");

            timeClicked = g_timeInMilliseconds;
            SetState(IContainerState::Clicked);

            logger->Info("we must call on click now");

            onClick->Emit();
        });
}

bool Container::CanBeClicked()
{
    if (canDrag)
        return true;

    if (onClick->GetListenersCount() == 0)
        return false;

    unsigned int now = g_timeInMilliseconds;

    return now - timeCreated > 60;
}

void Container::SetBackgroundImageIgnoreStyle(std::string bgFilePath)
{
    auto container = this;
    auto instanceId = _instanceId;
    auto cTag = tag;

    ExecuteWhenTexturesCanBeCreated(
        [container, instanceId, bgFilePath, cTag]()
        {
            if (container->GetInstanceId() != instanceId)
            {
                LOGE(
                    "Different ID! expected=%llu this=%llu (%s)", instanceId, container->GetInstanceId(), cTag.c_str());

                return;
            }

            MenuTexture *texture = CreateMenuTexture(bgFilePath, "button", false, COLOR_WHITE);

            container->_prevBackgroundImage = bgFilePath;
            container->SetBackgroundTexture(texture);
        });
}

void Container::SetBackgroundTexture(MenuTexture *texture)
{
    if (backgroundTexture != nullptr)
    {
        delete backgroundTexture;
        backgroundTexture = nullptr;
    }

    backgroundTexture = texture;
}

void Container::Dump()
{
    void *texture = nullptr;

    if (backgroundTexture)
        texture = backgroundTexture->sprite.m_pTexture;

    logger->Info(
        "Container %p | tag= %s | texture=%p | position=(%.2f, %.2f) | size=(%.2f, %.2f) | scale=(%.2f, %.2f) | "
        "children=%zu | text=%s",
        this, tag.c_str(), texture, currentPosition.x, currentPosition.y, currentSize.x, currentSize.y, currentScale.x,
        currentScale.y, children.size(), text.c_str());

    // for (Container *child : children)
    // {
    //     if (child)
    //         child->Dump();
    // }
}

void Container::SetState(IContainerState newState)
{
    if (state == newState)
        return;

    state = newState;

    logger->Info("calling onStateChanged");

    onStateChanged->Emit(state);
}

void Container::SetDisabled(bool disabled)
{
    if (disabled)
    {
        SetState(IContainerState::Disabled);
    }
    else
    {
        if (state == IContainerState::Clicked)
            return;

        SetState(IContainerState::Normal);
    }
}

void Container::SetBlocked(bool blocked)
{
    auto children = GetChildrenRecursive();

    if (blocked)
    {
        block = IBlockType::BlockThisAndChildren;

        for (auto child : children)
        {
            child->block = IBlockType::BlockInheritedFromParent;
        }
    }
    else
    {
        block = IBlockType::None;

        for (auto child : children)
        {
            child->block = IBlockType::None;
        }
    }
}

Container *Container::CreateContainer(std::string tag)
{
    Container *container = new Container(tag);
    return container;
}