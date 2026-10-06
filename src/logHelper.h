#pragma once

#include "mod/logger.h"
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

#include "menuOperation.h"

struct ScriptOperation
{
    unsigned int scriptId;
    unsigned int operationCode;

    std::string description;
    std::string result;

    bool completed = false;
};

class LogHelper
{
public:
    static bool bLog;
    static bool bDuringTouchEvent;

    static void Initialize(const std::string& modName);

    static void BeginOperation(unsigned int operationCode, std::string operationName, std::string description = "");
    static void EndOperation(unsigned int operationCode, std::string result = "");

    static void CreateLogFile();
    static void RemoveOldCrashLogs();

    static void OnLoggerMessage(eLogPrio prio, const char* msg);
    static void OnLoggerMessage(eLogPrio prio, const std::string& msg);

    static void DumpOperations();

private:
    static constexpr size_t MAX_FIRST_MESSAGES = 200;
    static constexpr size_t MAX_MESSAGES = 1000;
    static constexpr size_t MAX_CRASH_LOGS = 3;

    static std::string _modName;
    static std::string _logsFolder;

    static std::vector<std::string> _firstMessages;
    static std::vector<std::string> _messages;
    static size_t _messageIndex;

    static std::mutex _operationsMutex;
    static std::deque<ScriptOperation> _operations;
    static std::vector<ScriptOperation*> _pendingOperations;

    static std::unordered_map<uint64_t, std::string> _operationNames;

    static constexpr size_t MAX_OPERATIONS = 100;
};

#define BEGIN_OPERATION(operation) LogHelper::BeginOperation(operation, "MenuSZK_" #operation, "")
#define BEGIN_OPERATION_DESC(operation, description) LogHelper::BeginOperation(operation, "MenuSZK_" #operation, description)
#define END_OPERATION(operation) LogHelper::EndOperation(operation)
#define END_OPERATION_RESULT(operation, result) LogHelper::EndOperation(operation, result)