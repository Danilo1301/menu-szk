#pragma once

#include "string"

#include <cstdlib>

#pragma once

#include <cstdlib>
#include <string>

class CSSValue
{
  private:
    std::string oldValue = "";
    float oldParentSize = -1.0f;
    float parsedValue = 0.0f;

  public:
    static float StaticParse(const std::string &value, float parentSize)
    {
        if (value == "auto")
            return 0.0f;

        char *end = nullptr;

        float number = std::strtof(value.c_str(), &end);

        if (end == value.c_str())
            return 0.0f;

        if (!value.empty() && value.back() == '%')
            return number / 100.0f * parentSize;

        return number;
    }

    float Parse(const std::string &value, float parentSize)
    {
        if (value == oldValue && parentSize == oldParentSize)
            return parsedValue;

        oldValue = value;
        oldParentSize = parentSize;
        parsedValue = StaticParse(value, parentSize);

        return parsedValue;
    }
};