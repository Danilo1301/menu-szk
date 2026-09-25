#include "quickConfig.h"

#include <fstream>

QuickConfig::QuickConfig(const std::string &path) : _path(path)
{
    std::ifstream file(_path);

    if (!file.is_open())
        return;

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        size_t separator = line.find('=');

        if (separator == std::string::npos)
            continue;

        std::string name = line.substr(0, separator);
        std::string value = line.substr(separator + 1);

        _values[name] = value;
    }
}

void QuickConfig::AddBool(const std::string &name, bool defaultValue)
{
    auto it = _values.find(name);

    if (it != _values.end())
        _bools[name] = it->second == "true" || it->second == "1";
    else
        _bools[name] = defaultValue;
}

void QuickConfig::AddInt(const std::string &name, int defaultValue)
{
    auto it = _values.find(name);

    if (it == _values.end())
    {
        _ints[name] = defaultValue;
        return;
    }

    char *end = nullptr;
    long value = std::strtol(it->second.c_str(), &end, 10);

    if (end == it->second.c_str() || *end != '\0')
    {
        _ints[name] = defaultValue;
        return;
    }

    _ints[name] = static_cast<int>(value);
}

void QuickConfig::AddFloat(const std::string &name, float defaultValue)
{
    auto it = _values.find(name);

    if (it == _values.end())
    {
        _floats[name] = defaultValue;
        return;
    }

    char *end = nullptr;
    float value = std::strtof(it->second.c_str(), &end);

    if (end == it->second.c_str() || *end != '\0')
    {
        _floats[name] = defaultValue;
        return;
    }

    _floats[name] = value;
}

void QuickConfig::AddString(const std::string &name, const std::string &defaultValue)
{
    auto it = _values.find(name);

    if (it != _values.end())
        _strings[name] = it->second;
    else
        _strings[name] = defaultValue;
}

bool *QuickConfig::GetBool(const std::string &name)
{
    auto it = _bools.find(name);

    if (it == _bools.end())
        return nullptr;

    return &it->second;
}

int *QuickConfig::GetInt(const std::string &name)
{
    auto it = _ints.find(name);

    if (it == _ints.end())
        return nullptr;

    return &it->second;
}

float *QuickConfig::GetFloat(const std::string &name)
{
    auto it = _floats.find(name);

    if (it == _floats.end())
        return nullptr;

    return &it->second;
}

std::string *QuickConfig::GetString(const std::string &name)
{
    auto it = _strings.find(name);

    if (it == _strings.end())
        return nullptr;

    return &it->second;
}

void QuickConfig::Save()
{
    std::ofstream file(_path);

    if (!file.is_open())
        return;

    file << "# QuickConfig\n\n";

    for (const auto &[name, value] : _bools)
        file << name << "=" << (value ? "true" : "false") << "\n";

    for (const auto &[name, value] : _ints)
        file << name << "=" << value << "\n";

    for (const auto &[name, value] : _floats)
        file << name << "=" << value << "\n";

    for (const auto &[name, value] : _strings)
        file << name << "=" << value << "\n";
}