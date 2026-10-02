#include "logHelper.h"

#include "mod/iaml.h"
#include "mod/logger.h"
#include "utils/utils.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <deque>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <vector>

bool LogHelper::bLog = false;
bool LogHelper::bDuringTouchEvent = false;

std::string LogHelper::_modName = "";
std::string LogHelper::_logsFolder = "";

std::vector<std::string> LogHelper::_firstMessages;
std::vector<std::string> LogHelper::_messages;
size_t LogHelper::_messageIndex = 0;
std::mutex LogHelper::_operationsMutex;
std::deque<ScriptOperation> LogHelper::_operations;
std::vector<ScriptOperation*> LogHelper::_pendingOperations;
std::unordered_map<uint64_t, std::string> LogHelper::_operationNames;

void LogHelper::OnLoggerMessage(eLogPrio prio, const char* msg)
{
    if (!msg) return;

    OnLoggerMessage(prio, std::string(msg));
}

void LogHelper::OnLoggerMessage(eLogPrio prio, const std::string& msg)
{
    try
    {
        if (_firstMessages.size() < MAX_FIRST_MESSAGES) _firstMessages.emplace_back(msg);

        if (_messages.size() < MAX_MESSAGES)
        {
            _messages.emplace_back(msg);
            return;
        }

        _messages[_messageIndex] = msg;
        _messageIndex = (_messageIndex + 1) % MAX_MESSAGES;
    }
    catch (...)
    {
        // Nunca deixar o callback de logging derrubar o processo.
    }
}

void LogHelper::Initialize(const std::string& modName)
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

    _operations.clear();
    _pendingOperations.clear();

    _operationNames.clear();

    logger->SetMessageCB(OnLoggerMessage);
    logger->Info("LogHelper initialized");
}

void LogHelper::BeginOperation(unsigned int operationCode, std::string operationName, std::string description)
{
    std::lock_guard<std::mutex> lock(_operationsMutex);

    _operationNames.try_emplace(operationCode, std::move(operationName));

    while (_operations.size() >= MAX_OPERATIONS && !_operations.empty() && _operations.front().completed) { _operations.pop_front(); }

    ScriptOperation operation{};

    operation.operationCode = operationCode;
    operation.description = std::move(description);
    operation.completed = false;

    _operations.push_back(std::move(operation));

    _pendingOperations.push_back(&_operations.back());
}

void LogHelper::EndOperation(unsigned int operationCode, std::string result)
{
    std::lock_guard<std::mutex> lock(_operationsMutex);

    for (auto it = _pendingOperations.rbegin(); it != _pendingOperations.rend(); ++it)
    {
        ScriptOperation* operation = *it;

        if (operation->operationCode != operationCode) continue;

        operation->result = std::move(result);
        operation->completed = true;

        _pendingOperations.erase(std::next(it).base());

        return;
    }

    auto nameIt = _operationNames.find(operationCode);

    const char* operationName = nameIt != _operationNames.end() ? nameIt->second.c_str() : "Unknown";

    LOGE("[OPERATION] Operation not found name=%s code=%u pending=%zu", operationName, operationCode, _pendingOperations.size());

    DumpOperations();
}

void LogHelper::RemoveOldCrashLogs()
{
    namespace fs = std::filesystem;

    std::vector<fs::directory_entry> logs;

    if (!fs::exists(_logsFolder)) return;

    const std::string prefix = "crash_" + _modName + "_";

    for (const auto& entry : fs::directory_iterator(_logsFolder))
    {
        if (!entry.is_regular_file()) continue;

        const std::string filename = entry.path().filename().string();

        if (filename.rfind(prefix, 0) == 0 && entry.path().extension() == ".log") { logs.emplace_back(entry); }
    }

    if (logs.size() <= MAX_CRASH_LOGS) return;

    std::sort(logs.begin(),
        logs.end(),
        [](const fs::directory_entry& a, const fs::directory_entry& b) { return fs::last_write_time(a) < fs::last_write_time(b); });

    while (logs.size() > MAX_CRASH_LOGS)
    {
        std::error_code error;
        fs::remove(logs.front().path(), error);

        logs.erase(logs.begin());
    }
}

void LogHelper::CreateLogFile()
{
    if (LogHelper::bDuringTouchEvent) { __android_log_print(ANDROID_LOG_ERROR, "PSDK", " ** WARNING ** Crashed during touch event"); }

    if (_logsFolder.empty()) { return; }

    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};
    localtime_r(&time, &localTime);

    std::ostringstream filename;
    filename << _logsFolder << "crash_" << _modName << "_" << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S") << ".log";

    FILE* file = fopen(filename.str().c_str(), "w");

    if (!file) { return; }

    __android_log_print(ANDROID_LOG_ERROR, "PSDK", "CRASH");

    fprintf(file, "========== CRASH LOG ==========\n\n");
    fprintf(file, "modName = %s\n", _modName.c_str());

    // ---------------------------------------------------------
    // Operations
    // ---------------------------------------------------------

    fprintf(file, "\n========== OPERATIONS ==========\n\n");

    size_t pendingOperations = 0;
    size_t completedOperations = 0;

    for (const ScriptOperation& operation : _operations)
    {
        auto nameIt = _operationNames.find(operation.operationCode);

        const char* operationName = nameIt != _operationNames.end() ? nameIt->second.c_str() : "Unknown";

        std::string descriptionText = operation.description.empty() ? "" : " (description: " + operation.description + ")";

        std::string resultText = operation.result.empty() ? "" : " (result: " + operation.result + ")";

        if (operation.completed)
        {
            completedOperations++;

            fprintf(file,
                "[COMPLETED] %s (code: %u)%s%s\n",
                operationName,
                operation.operationCode,
                descriptionText.c_str(),
                resultText.c_str());

            continue;
        }

        pendingOperations++;

        __android_log_print(
            ANDROID_LOG_ERROR, "PSDK", "[PENDING] %s (code: %u)%s", operationName, operation.operationCode, descriptionText.c_str());

        fprintf(file, "[PENDING] %s (code: %u)%s\n", operationName, operation.operationCode, descriptionText.c_str());
    }

    fprintf(file, "\nOperations: %zu pending, %zu completed\n", pendingOperations, completedOperations);

    // ---------------------------------------------------------
    // First messages
    // ---------------------------------------------------------

    fprintf(file, "\n========== FIRST 200 LOG MESSAGES ==========\n\n");

    for (const std::string& message : _firstMessages) { fprintf(file, "%s\n", message.c_str()); }

    // ---------------------------------------------------------
    // Last messages
    // ---------------------------------------------------------

    fprintf(file, "\n========== LAST 1000 LOG MESSAGES ==========\n\n");

    if (_messages.size() < MAX_MESSAGES)
    {
        for (const std::string& message : _messages) { fprintf(file, "%s\n", message.c_str()); }
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
}

void LogHelper::DumpOperations()
{
    __android_log_print(ANDROID_LOG_INFO, "MenuSZK-PSDK", "========== OPERATIONS DUMP ==========");

    __android_log_print(ANDROID_LOG_INFO, "MenuSZK-PSDK", "History: %zu | Pending: %zu", _operations.size(), _pendingOperations.size());

    size_t index = 0;

    for (const ScriptOperation& operation : _operations)
    {
        auto nameIt = _operationNames.find(operation.operationCode);

        const char* operationName = nameIt != _operationNames.end() ? nameIt->second.c_str() : "Unknown";

        __android_log_print(ANDROID_LOG_INFO,
            "MenuSZK-PSDK",
            "[%zu] %s code=%u status=%s description=%s result=%s",
            index++,
            operationName,
            operation.operationCode,
            operation.completed ? "COMPLETED" : "PENDING",
            operation.description.c_str(),
            operation.result.c_str());
    }

    __android_log_print(ANDROID_LOG_INFO, "MenuSZK-PSDK", "========== END OPERATIONS DUMP ==========");
}