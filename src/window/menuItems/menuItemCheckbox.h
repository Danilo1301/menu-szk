#pragma once

#include "../menuItem.h"

class MenuItemCheckbox : public MenuItem
{
  private:
    bool *pBool;
    Container *checkboxContainer;

  public:
    MenuItemCheckbox(Window *window, bool *ptr);
    ~MenuItemCheckbox();

    void UpdateCheckbox();

    virtual bool GetBoolValue() override { return *pBool; }
};