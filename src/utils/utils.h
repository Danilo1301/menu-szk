#pragma once

#include "../config.h"
#include "mod/logger.h"
#include "src/pch.h"

#include <chrono>
#include <fstream>
#include <thread>

#ifndef OBFUSCATION_KEY
#define OBFUSCATION_KEY "test"
#endif

#define SECRET_FILE_VERSION "01"

inline std::string StringToHex(const std::string& value)
{
    static constexpr char hex[] = "0123456789ABCDEF";

    std::string result;
    result.reserve(value.size() * 2);

    for (unsigned char c : value)
    {
        result += hex[c >> 4];
        result += hex[c & 0x0F];
    }

    return result;
}

inline std::string ObfuscateSecret(const std::string& value)
{
    const std::string content = StringToHex(SECRET_FILE_VERSION) + "\n" + value;

    const char key[] = OBFUSCATION_KEY;

    std::string result = content;

    for (size_t i = 0; i < result.size(); i++) result[i] ^= key[i % (sizeof(key) - 1)];

    return result;
}

inline std::string DeobfuscateSecret(const std::string& value)
{
    const std::string result = ObfuscateSecret(value);

    const size_t separator = result.find('\n');

    if (separator == std::string::npos) return "";

    std::string fileVersion = result.substr(0, separator);

    if (!fileVersion.empty() && fileVersion.back() == '\r') fileVersion.pop_back();

    if (fileVersion != StringToHex(SECRET_FILE_VERSION)) return "";

    return result.substr(separator + 1);
}

inline std::string DownloadAndGetContent(std::string url)
{
    std::string savePath = GetMenuFolder() + "/tmp.downloadedjson";

    logger->Info("Downloading from %s", url.c_str());

    if (!aml->DownloadFile(url.c_str(), savePath.c_str()))
    {
        logger->Info("Download failed");
        return "";
    }

    std::ifstream file(savePath);

    if (!file.is_open())
    {
        logger->Info("Failed to open downloaded file");
        remove(savePath.c_str());
        return "";
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();
    remove(savePath.c_str());

    return content;
}

inline void SetTimeout(std::function<void()> callback, int milliseconds)
{
    std::thread(
        [callback, milliseconds]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));

            callback();
        })
        .detach();
}

inline bool FileExists(const std::string& path)
{
    std::ifstream f(path.c_str());
    return f.good();
}

inline void CreateFullPath(const std::string& path)
{
    std::error_code error;
    std::filesystem::create_directories(path, error);

    if (error) { LOGE("Failed to create folder: %s (%s)", path.c_str(), error.message().c_str()); }
}

inline void JustCreateFile(const std::string& path)
{
    std::ofstream file(path);
}

inline void RemoveFile(const std::string& path)
{
    std::error_code error;

    std::filesystem::remove(path, error);

    if (error) { LOGE("Failed to remove file: %s (%s)", path.c_str(), error.message().c_str()); }
}

inline void PrintFileContent(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        logger->Info("Failed to open file: %s", path.c_str());
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    logger->Info("File content:\n%s", content.c_str());
}