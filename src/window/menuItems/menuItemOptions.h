#pragma once

#include "../menuItem.h"

struct OptionsContainers
{
    Container *left;
    Container *right;
    Container *value;
};

enum class OptionsType
{
    None,
    List,
    Integer,
    Float
};

class MenuItemOptions : public MenuItem
{
  private:
    OptionsContainers containers;

    OptionsType type = OptionsType::None;

    std::vector<std::pair<int, std::string>> options;
    int optionsIndex = 0;

    int *integerValue = nullptr;
    int integerMin = 0;
    int integerMax = 0;
    int integerStep = 1;

    float *floatValue = nullptr;
    float floatMin = 0.0f;
    float floatMax = 0.0f;
    float floatStep = 0.1f;

  public:
    MenuItemOptions(Window *window, float optionsWidth);
    ~MenuItemOptions();

    void AddOption_Internal(int value, std::string text);

    void SetInteger(int *value, int min, int max, int step = 1);
    void SetFloat(float *value, float min, float max, float step = 0.1f);

    void UpdateVisuals(bool leftState, bool rightState);

    void UpdateOptionsValue();

    void InvokeOnChangedValue();

    virtual void AddOption(int value, std::string displayText) override { AddOption_Internal(value, displayText); }

    virtual int GetCurrentOptionValue() override
    {
        if (type == OptionsType::List)
        {
            if (optionsIndex < 0 || optionsIndex >= options.size())
            {
                LOGE("Invalid optionsIndex: %d (size: %d)", optionsIndex, options.size());
                return -1;
            }
        }
        if (type == OptionsType::Integer)
        {
            return *integerValue;
        }
        if (type == OptionsType::Float)
        {
            LOGE("Cant get value from menuItem type FLOAT");
            return (int)(*floatValue);
        }
        LOGE("Cant get value from menuItem type UNKNOWN");
        return -1;
    }

    static OptionsContainers CreateOptionsContainers(Container *root, float optionsWidth);
};