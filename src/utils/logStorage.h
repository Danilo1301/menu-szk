#pragma once

#include "mod/iaml.h"
#include "mod/logger.h"

#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

static constexpr size_t MAX_MESSAGES = 1000;

static std::vector<std::string> g_lastMessages;
static size_t g_messageIndex = 0;

// static struct sigaction g_oldActions[5];
// static volatile sig_atomic_t g_handlingCrash = 0;

// static const int g_signals[5] = {SIGSEGV, SIGABRT, SIGBUS, SIGFPE, SIGILL};

inline void OnLoggerMessage(eLogPrio prio, const char *msg)
{
    if (!msg)
        return;

    if (g_lastMessages.size() < MAX_MESSAGES)
    {
        g_lastMessages.emplace_back(msg);
        return;
    }

    g_lastMessages[g_messageIndex] = msg;
    g_messageIndex = (g_messageIndex + 1) % MAX_MESSAGES;
}

inline std::string GetCrashFilePath()
{
    std::string dataRootPath = aml->GetAndroidDataRootPath();

    return dataRootPath + "/mods/data/menuSZK/lastcrash.log";
}

inline void SaveToFile()
{
    auto path = GetCrashFilePath();

    FILE *file = fopen(path.c_str(), "w");

    if (!file)
        return;

    fprintf(file, "========== MENU SZK CRASH LOG ==========\n\n");

    if (g_lastMessages.size() < MAX_MESSAGES)
    {
        for (const auto &message : g_lastMessages)
            fprintf(file, "%s\n", message.c_str());
    }
    else
    {
        for (size_t i = 0; i < MAX_MESSAGES; i++)
        {
            size_t index = (g_messageIndex + i) % MAX_MESSAGES;

            fprintf(file, "%s\n", g_lastMessages[index].c_str());
        }
    }

    fclose(file);
}

inline void InitLogStorage()
{
    logger->SetMessageCB(OnLoggerMessage);
    logger->Info("LogStorage initialized");
}