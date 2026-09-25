#include "screenDebug.h"
#include "../utils/drawUtils.h"
#include "aml-psdk/gta_base/Vector.h"
#include "menu/menu.h"
#include <cstddef>
#include <string>

ScreenDebug::ScreenDebug(int maxLines) : _maxLines(maxLines) {}

ScreenDebug *ScreenDebug::Main = new ScreenDebug();

void ScreenDebug::AddLine(const std::string &line)
{
    std::string trimmed = line;

    trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));

    trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);

    if (_lines.size() >= _maxLines)
        _lines.erase(_lines.begin());

    _lines.push_back(trimmed);
}

void ScreenDebug::AddLine(const char *format, ...)
{
    char buffer[1024];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    AddLine(buffer);
}

void ScreenDebug::Draw()
{
    IFont font;
    font.size = 1.5f;

    const float lineHeight = 40.0f;
    const CVector2D startPosition(20.0f, 20.0f);

    for (int i = 0; i < _lines.size(); i++)
    {
        CVector2D position(startPosition.x, startPosition.y + (i * lineHeight));

        DrawUtils::DrawText(_lines[i], position, font, CVector2D(1, 1), false, 1);
    }
}

void ScreenDebug::Clear() { _lines.clear(); }