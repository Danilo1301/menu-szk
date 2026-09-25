#pragma once

#include "../config.h"
#include "mod/logger.h"

#include <chrono>
#include <fstream>
#include <thread>

#ifndef OBFUSCATION_KEY
#define OBFUSCATION_KEY "test"
#endif

inline std::string ObfuscateSecret(const std::string &value)
{
    const char key[] = OBFUSCATION_KEY;

    std::string result = value;

    for (size_t i = 0; i < result.size(); i++)
        result[i] ^= key[i % (sizeof(key) - 1)];

    return result;
}

inline std::string DeobfuscateSecret(const std::string &value) { return ObfuscateSecret(value); }

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