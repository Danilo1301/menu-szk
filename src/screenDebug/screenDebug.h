#pragma once

#include <string>
#include <vector>

class ScreenDebug
{
  private:
    std::vector<std::string> _lines;
    int _maxLines;

  public:
    ScreenDebug(int maxLines = 20);

    bool onlyLogErrors = true;

    void AddLine(const std::string &line);
    void AddLine(const char *format, ...);
    void Draw();
    void Clear();

    static ScreenDebug *Main;
};