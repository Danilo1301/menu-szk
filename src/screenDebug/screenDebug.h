#pragma once

#include "menuSZK/imenuSZK.h"

#include <string>
#include <vector>

struct ScreenLogLine
{
    ScreenLogType type = ScreenLogType::Info;
    std::string text;
    int displayTime = 5000;
    unsigned int timeCreated;
};

class ScreenDebug
{
private:
    std::vector<ScreenLogLine> _lines;
    int _maxStoreLines = 300;
    int _maxDrawLines;

public:
    ScreenDebug(int maxDrawLines = 20);

    bool hideInfoMessages = true;

    void AddLine(const std::string& line, ScreenLogType type = ScreenLogType::Info, int displayTime = 5000);
    void Info(const std::string& line);
    void Warn(const std::string& line);
    void Error(const std::string& line);

    void Draw();
    void Clear();

    static ScreenDebug* Main;
};