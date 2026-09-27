#include "logHelper.h"

#include "mod/iaml.h"
#include "utils/utils.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <vector>

std::string LogHelper::_modName = "";
std::string LogHelper::_logsFolder = "";
std::string LogHelper::_lastFrameOperation = "none";

std::vector<std::string> LogHelper::_firstMessages;
std::vector<std::string> LogHelper::_messages;
size_t LogHelper::_messageIndex = 0;

void LogHelper::OnLoggerMessage(eLogPrio prio, const char *msg)
{
    // if (!msg)
    // return;

    if (_firstMessages.size() < MAX_FIRST_MESSAGES)
        _firstMessages.emplace_back(msg);

    if (_messages.size() < MAX_MESSAGES)
    {
        _messages.emplace_back(msg);
        return;
    }

    _messages[_messageIndex] = msg;
    _messageIndex = (_messageIndex + 1) % MAX_MESSAGES;
}

void LogHelper::Initialize(const std::string &modName)
{
    _modName = modName;

    std::string dataRootPath = aml->GetAndroidDataRootPath();
    _logsFolder = dataRootPath + "/mods/data/logs/";

    CreateFullPath(_logsFolder);

    _firstMessages.clear();
    _firstMessages.reserve(MAX_FIRST_MESSAGES);

    _messages.clear();
    _messages.reserve(MAX_MESSAGES);

    _messageIndex = 0;

    logger->SetMessageCB(OnLoggerMessage);
    logger->Info("LogHelper initialized");
}

void LogHelper::SetFrameOperation(const std::string &description) { _lastFrameOperation = description; }

void LogHelper::RemoveOldCrashLogs()
{
    namespace fs = std::filesystem;

    std::vector<fs::directory_entry> logs;

    if (!fs::exists(_logsFolder))
        return;

    const std::string prefix = "crash_" + _modName + "_";

    for (const auto &entry : fs::directory_iterator(_logsFolder))
    {
        if (!entry.is_regular_file())
            continue;

        const std::string filename = entry.path().filename().string();

        if (filename.rfind(prefix, 0) == 0 && entry.path().extension() == ".log")
        {
            logs.emplace_back(entry);
        }
    }

    if (logs.size() <= MAX_CRASH_LOGS)
        return;

    std::sort(logs.begin(), logs.end(), [](const fs::directory_entry &a, const fs::directory_entry &b)
        { return fs::last_write_time(a) < fs::last_write_time(b); });

    while (logs.size() > MAX_CRASH_LOGS)
    {
        std::error_code error;
        fs::remove(logs.front().path(), error);

        logs.erase(logs.begin());
    }
}

void LogHelper::CreateLogFile()
{
    if (_logsFolder.empty())
        return;

    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};

    // #if defined(__ANDROID__)
    localtime_r(&time, &localTime);
    // #else
    // localTime = *std::localtime(&time);
    // #endif

    std::ostringstream filename;
    filename << _logsFolder << "crash_" << _modName << "_" << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S") << ".log";

    FILE *file = fopen(filename.str().c_str(), "w");

    if (!file)
        return;

    fprintf(file, "========== CRASH LOG ==========\n\n");

    fprintf(file, "modName = %s\n", _modName.c_str());
    fprintf(file, "lastOperation = %s\n\n", _lastFrameOperation.c_str());

    fprintf(file, "========== FIRST 200 LOG MESSAGES ==========\n\n");

    for (const std::string &message : _firstMessages)
    {
        fprintf(file, "%s\n", message.c_str());
    }

    fprintf(file, "\n========== LAST 1000 LOG MESSAGES ==========\n\n");

    if (_messages.size() < MAX_MESSAGES)
    {
        for (const std::string &message : _messages)
        {
            fprintf(file, "%s\n", message.c_str());
        }
    }
    else
    {
        // _messageIndex aponta para a mensagem mais antiga.
        for (size_t i = 0; i < MAX_MESSAGES; i++)
        {
            size_t index = (_messageIndex + i) % MAX_MESSAGES;
            fprintf(file, "%s\n", _messages[index].c_str());
        }
    }

    fprintf(file, "\n========== ========= ==========\n");

    fclose(file);

    RemoveOldCrashLogs();
}