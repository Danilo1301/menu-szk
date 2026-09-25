#include "slider.h"

#include "../config.h"
#include "../container/containerLoader.h"

#include "../input.h"
#include <string>

Slider::Slider(Container *container, float *ptr, float minValue, float maxValue, int decimals)
{
    _container = container;
    pValue = ptr;
    _minValue = minValue;
    _maxValue = maxValue;
    _decimalsAmount = decimals;

    LoadContainerFromFile(container, GetMenuLayoutPath("slider.json"));

    //_container->style.left = "300px";
    //_container->style.right = "auto";
    //_container->style.top = "300px";
    //_container->style.bottom = "auto";

    _container->canBlockTouchEvents = true;

    auto area = _container->FindChild("area");
    auto handler = _container->FindChild("handler");

    auto text = _container->FindChild("text");

    // to make it clickable
    area->onClick->Add([]() {});
    handler->onClick->Add([]() {});

    area->onPreUpdateTransform->Add(
        [this, area, handler]()
        {
            float width = area->GetCurrentSize().x;

            _areaWidth = width;

            std::unordered_map<int, InputTouch> *touches = Input::GetTouches();

            if (touches->empty())
                return;

            auto touch0 = (*touches)[0];
            auto touchPosition = touch0.position;

            if (area->GetCurrentTrackingTouchId() == -1 && handler->GetCurrentTrackingTouchId() == -1)
                return;

            auto mainContainerLocalPosition = touch0.position;

            auto areaLocalPosition = Container::MainContainer->LocalToOther(mainContainerLocalPosition, area);

            float localX = std::max(0.0f, std::min(width, areaLocalPosition.x));

            float value01 = (localX / width);

            SetValue01(value01);
        });

    area->onPostUpdateTransform->Add(
        [this, handler, text]()
        {
            float value = *pValue;

            float value01 = (value - _minValue) / (_maxValue - _minValue);

            float localX = value01 * _areaWidth;

            handler->style.left = std::to_string(localX) + "px";
            handler->style.right = "auto";
            handler->style.top = "0px";
            handler->style.bottom = "auto";

            std::ostringstream stream;
            stream << std::fixed << std::setprecision(_decimalsAmount) << value;

            text->text = stream.str();
        });
}

void Slider::SetValue01(float value01)
{
    value01 = std::max(0.0f, std::min(1.0f, value01));

    SetValue(_minValue + value01 * (_maxValue - _minValue));
}

void Slider::SetValue(float _value)
{
    _value = std::max(_minValue, std::min(_maxValue, _value));

    float multiplier = std::pow(10.0f, _decimalsAmount);

    *pValue = std::trunc(_value * multiplier) / multiplier;

    if (onValueChange)
        onValueChange();
}

Container *Slider::GetContainer() { return _container; }