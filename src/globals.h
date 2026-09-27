#pragma once

#include "mod/logger.h"

#include "vector"

#define LOG_PER_FRAME_ENABLED false
#define LOG_INPUTS false

#define SHOW_LEAK_MESSAGES false

// unused
inline std::vector<std::string> leakUtils_showOnlyItems = {"Container"};

inline void LOG_PER_FRAME(std::string text)
{
    if (!LOG_PER_FRAME_ENABLED)
        return;

    logger->Info("%s", text.c_str());
}

inline void LOG_INPUT(std::string text)
{
    if (!LOG_INPUTS)
        return;

    logger->Info("%s", text.c_str());
}