#pragma once

#include "mod/iaml.h"
#include <string>

inline std::string GetMenuFolder()
{
    std::string dataRootPath = aml->GetAndroidDataRootPath();

    std::string folderName = "menuSZK";

    std::string menuFolder = dataRootPath + "/mods/data/" + folderName + "/";

    return menuFolder;
}

inline std::string GetMenuAssetPath(std::string relativePath)
{
    std::string menuFolder = GetMenuFolder();

    return menuFolder + "/assets/" + relativePath;
}

inline std::string GetMenuLayoutPath(std::string relativePath)
{
    std::string menuFolder = GetMenuFolder();

    return menuFolder + "/layouts/" + relativePath;
}