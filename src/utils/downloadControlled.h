#pragma once

#include "mod/iaml.h"
#include "mod/logger.h"
#include "../config.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>

inline std::string GetObfuscatedFileName(const std::string& storeTo)
{
    const size_t hash = std::hash<std::string>{}(storeTo);

    std::stringstream stream;
    stream << std::hex << hash;

    return stream.str();
}

inline bool DownloadIfPossible(const std::string& url, const std::string& storeTo)
{
    constexpr long long DOWNLOAD_INTERVAL_MINUTES = 30;
    constexpr long long DOWNLOAD_INTERVAL = DOWNLOAD_INTERVAL_MINUTES * 60;

    const std::string cacheFolder = GetMenuFolder() + "/cache";
    const std::string metadataFile = cacheFolder + "/" + GetObfuscatedFileName(storeTo) + ".meta";

    std::filesystem::create_directories(std::filesystem::path(storeTo).parent_path());

    std::filesystem::create_directories(cacheFolder);

    const long long currentTime =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    long long lastDownloadTime = 0;
    long long lastFileSize = -1;

    {
        std::ifstream file(metadataFile);

        if (file) file >> lastDownloadTime >> lastFileSize;
    }

    const bool fileExists = std::filesystem::exists(storeTo);

    if (fileExists && lastDownloadTime > 0 && currentTime - lastDownloadTime < DOWNLOAD_INTERVAL) { return false; }

    if (!aml->DownloadFile(url.c_str(), storeTo.c_str()))
    {
        logger->Info("File errored");
        return false;
    }

    const long long newFileSize = static_cast<long long>(std::filesystem::file_size(storeTo));

    std::ofstream file(metadataFile, std::ios::trunc);
    file << currentTime << " " << newFileSize;

    logger->Info("File downloaded! Size: %lld bytes", newFileSize);

    return true;
}