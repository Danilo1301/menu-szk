#pragma once

#include "mod/logger.h"

#include "../global_config.h"
#include <string>
#include <unordered_map>


class LeakUtils
{
  public:
    static void RegisterItem(const std::string &type)
    {
        _counts[type]++;

        if (SHOW_LEAK_MESSAGES)
        {
            logger->Info("[leak] created item: %s, total: %d", type.c_str(), _counts[type]);
        }
    }

    static void FreeItem(const std::string &type)
    {
        auto it = _counts.find(type);

        if (it == _counts.end())
            return;

        it->second--;

        if (SHOW_LEAK_MESSAGES)
        {
            logger->Info("[leak] free item: %s, total: %d", type.c_str(), it->second);
        }

        if (it->second <= 0)
            _counts.erase(it);
    }

  private:
    inline static std::unordered_map<std::string, int> _counts;
};