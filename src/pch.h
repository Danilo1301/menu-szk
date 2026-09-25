#pragma once

#include "global_config.h"

#include <mod/amlmod.h>
#include <mod/config.h>
#include <mod/logger.h>

#include <cstddef>

#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// #include <string>
#include <dlfcn.h>
#include <pthread.h>

// #include <map>

#include <android/log.h>

// #include "include/gta_utils.h"

#include "aml-psdk/gta_base/RGBA.h"
#include "aml-psdk/gta_base/Rect.h"
#include "aml-psdk/gta_base/Vector.h"

// menu
#include "menu/menu.h"

#include "utils/leakUtils.h"

#include "screenDebug/screenDebug.h"

inline void LOGI(const char *format, ...)
{
    char buffer[1024];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    logger->Info("%s", buffer);

    if (ScreenDebug::Main->onlyLogErrors == false)
        ScreenDebug::Main->AddLine(std::string(buffer));
}

inline void LOGE(const char *format, ...)
{
    char buffer[1024];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    logger->Error("%s", buffer);

    ScreenDebug::Main->AddLine("~r~" + std::string(buffer));
}

inline unsigned int g_timeInMilliseconds = 0;
inline unsigned int g_deltaTime = 0;
static unsigned int g_renderDeltaTime = 0;

inline unsigned int g_framesDrawn = 0;
inline bool g_gameHasFirstProcessed = false;

inline CRGBA COLOR_BLACK = CRGBA(0, 0, 0);
inline CRGBA COLOR_WHITE = CRGBA(255, 255, 255);