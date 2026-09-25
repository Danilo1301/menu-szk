#pragma once

#include <map>
#include <string>

class QuickConfig
{
  private:
    std::string _path;

    std::map<std::string, bool> _bools;
    std::map<std::string, int> _ints;
    std::map<std::string, float> _floats;
    std::map<std::string, std::string> _strings;

    std::map<std::string, std::string> _values;

  public:
    QuickConfig(const std::string &path);

    void AddBool(const std::string &name, bool defaultValue);
    void AddInt(const std::string &name, int defaultValue);
    void AddFloat(const std::string &name, float defaultValue);
    void AddString(const std::string &name, const std::string &defaultValue);

    bool *GetBool(const std::string &name);
    int *GetInt(const std::string &name);
    float *GetFloat(const std::string &name);
    std::string *GetString(const std::string &name);

    void Save();
};