#include "screenDebug.h"
#include "../utils/drawUtils.h"
#include "aml-psdk/gta_base/RGBA.h"
#include "aml-psdk/gta_base/Vector.h"
#include "menu/menu.h"
#include "src/pch.h"
#include <cstddef>
#include <string>

ScreenDebug::ScreenDebug(int maxLines) : _maxDrawLines(maxLines) {}

ScreenDebug *ScreenDebug::Main = new ScreenDebug();

void ScreenDebug::AddLine(const std::string &line, ScreenLogType type)
{
    if (_lines.size() >= _maxStoreLines)
        _lines.erase(_lines.begin());

    _lines.push_back({type, line});
}

void ScreenDebug::Info(const std::string &line) { AddLine(line, ScreenLogType::Info); }
void ScreenDebug::Warn(const std::string &line) { AddLine(line, ScreenLogType::Warning); }
void ScreenDebug::Error(const std::string &line) { AddLine(line, ScreenLogType::Error); }

void ScreenDebug::Draw()
{
    IFont font;
    font.size = 1.5f;

    const float lineHeight = 40.0f;
    const CVector2D startPosition(50.0f, 80.0f);

    int visibleLines = 0;
    int startIndex = static_cast<int>(_lines.size()) - 1;

    for (; startIndex >= 0; startIndex--)
    {
        const auto &line = _lines[startIndex];

        if (hideInfoMessages && line.type == ScreenLogType::Info)
            continue;

        visibleLines++;

        if (visibleLines >= _maxDrawLines)
            break;
    }

    int drawIndex = 0;

    for (int i = startIndex + 1; i < static_cast<int>(_lines.size()); i++)
    {
        const auto &line = _lines[i];

        if (hideInfoMessages && line.type == ScreenLogType::Info)
            continue;

        font.color = COLOR_WHITE;

        if (line.type == ScreenLogType::Error)
            font.color = CRGBA(255, 0, 0);

        if (line.type == ScreenLogType::Warning)
            font.color = CRGBA(255, 255, 0);

        CVector2D position(startPosition.x, startPosition.y + (drawIndex * lineHeight));

        DrawUtils::DrawText(line.text, position, font, CVector2D(1, 1), false, 1);

        drawIndex++;
    }
}

void ScreenDebug::Clear() { _lines.clear(); }