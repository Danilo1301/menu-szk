#include "localization.h"

#include "../utils/iniReaderWriter.hpp"

#include <filesystem>

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> Localization::lines;

std::string Localization::currentLanguage = "english"; // padrão

void Localization::RegisterLine(const std::string& language, const std::string& key, const std::string& value)
{
    auto& langMap = lines[language]; // pega o mapa de chaves desse idioma

    // se já existe, apenas loga e sai
    if (langMap.find(key) != langMap.end()) { return; }

    // se não existe, registra normalmente
    langMap[key] = value;
    //menuDebug->AddLine("[Localization] Added " + language + " - " + key);
}

void Localization::SetLanguage(const std::string& language)
{
    currentLanguage = language;
}

std::string Localization::Get(const std::string& key)
{
    auto langIt = lines.find(currentLanguage);
    if (langIt != lines.end())
    {
        auto& map = langIt->second;
        auto keyIt = map.find(key);
        if (keyIt != map.end()) { return keyIt->second; }
    }

    // fallback: tenta inglês
    auto fallbackIt = lines.find("en");
    if (fallbackIt != lines.end())
    {
        auto& map = fallbackIt->second;
        auto keyIt = map.find(key);
        if (keyIt != map.end()) { return keyIt->second; }
    }

    return "[MISSING:" + key + "]";
}

bool Localization::HasKey(const std::string& key)
{
    auto langIt = lines.find(currentLanguage);
    if (langIt == lines.end()) return false;
    return langIt->second.find(key) != langIt->second.end();
}

std::vector<std::string> Localization::GetLanguages()
{
    std::vector<std::string> result;
    result.reserve(lines.size());

    for (const auto& pair : lines)
    {
        result.push_back(pair.first); // o nome do idioma (ex: "english", "portuguese")
    }

    return result;
}

void Localization::RegisterLocalizationFile(const std::string& filePath)
{
    namespace fs = std::filesystem;

    const fs::path path(filePath);

    if (!fs::exists(path) || !fs::is_regular_file(path)) return;

    if (path.extension() != ".ini") return;

    const std::string language = path.stem().string();

    IniReaderWriter ini;

    if (!ini.LoadFromFile(path.string()))
    {
        LOGW("[Localization] Could not load %s", path.string().c_str());
        return;
    }

    auto data = *ini.GetData();

    for (const auto& [sectionName, section] : data)
    {
        for (const auto& [key, value] : section)
        {
            const std::string fullKey = sectionName.empty() ? key : sectionName + "." + key;

            RegisterLine(language, fullKey, value);
        }
    }
}

void Localization::RegisterLocalizationFolder(const std::string& folderPath)
{
    namespace fs = std::filesystem;

    if (!fs::exists(folderPath) || !fs::is_directory(folderPath))
    {
        LOGW("[Localization] Could not find %s", folderPath.c_str());
        return;
    }

    for (const auto& entry : fs::directory_iterator(folderPath))
    {
        if (!entry.is_regular_file()) continue;

        RegisterLocalizationFile(entry.path().string());
    }
}

void Localization::RegisterLocalizationRecursively(const std::string& localizationFolder)
{
    namespace fs = std::filesystem;

    if (!fs::exists(localizationFolder) || !fs::is_directory(localizationFolder))
    {
        LOGW("[Localization] Could not find %s", localizationFolder.c_str());
        return;
    }

    for (const auto& entry : fs::recursive_directory_iterator(localizationFolder))
    {
        if (!entry.is_regular_file()) continue;

        RegisterLocalizationFile(entry.path().string());
    }
}