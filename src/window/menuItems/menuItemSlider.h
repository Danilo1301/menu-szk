#pragma once

#include "../menuItem.h"

class MenuItemSlider : public MenuItem
{
  private:
  public:
    MenuItemSlider(Window *window, float *pValue, float minValue, float maxValue, int decimals);
    ~MenuItemSlider();
};