#pragma once

#include "../utils/eventListener.h"
#include <functional>

class Container;

class Slider
{
  private:
    Container *_container;

    float *pValue;

    float _areaWidth = 0;

    float _minValue = 10;
    float _maxValue = 100;
    int _decimalsAmount = 1;

  public:
    Slider(Container *container, float *pValue, float minValue, float maxValue, int decimals);

    std::function<void()> onValueChange = nullptr;

    void SetValue01(float value01);
    void SetValue(float value);

    Container *GetContainer();
};