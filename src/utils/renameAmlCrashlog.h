#pragma once

#include "mod/iaml.h"
#include "mod/logger.h"
#include <filesystem>

inline void RenameAmlLogIfExists()
{
    const std::string dataRootPath = aml->GetAndroidDataRootPath();
    const std::string logFile = dataRootPath + "/aml_crashlog.txt";
    const std::string previousLogFile = dataRootPath + "/aml_crashlog(previous).txt";

    if (!std::filesystem::exists(logFile)) return;

    std::error_code error;

    std::filesystem::remove(previousLogFile, error);

    if (error)
    {
        logger->Info("Failed to remove previous crash log");
        return;
    }

    std::filesystem::rename(logFile, previousLogFile, error);

    if (error)
    {
        logger->Info("Failed to rename previous crash log");
        return;
    }

    logger->Info("Previous AML crash log renamed");
}