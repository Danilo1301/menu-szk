#include "screenDebug.h"
#include "../utils/drawUtils.h"
#include "aml-psdk/gta_base/RGBA.h"
#include "aml-psdk/gta_base/Vector.h"
#include "menuSZK/imenuSZK.h"
#include "src/pch.h"
#include <cstddef>
#include <string>

ScreenDebug::ScreenDebug(int maxLines) : _maxDrawLines(maxLines)
{
}

ScreenDebug* ScreenDebug::Main = new ScreenDebug();

void ScreenDebug::AddLine(const std::string& line, ScreenLogType type, int displayTime)
{
    if (_lines.size() >= _maxStoreLines) _lines.erase(_lines.begin());

    _lines.push_back({ type, line, displayTime, g_timeInMilliseconds });
}

void ScreenDebug::Info(const std::string& line)
{
    AddLine(line, ScreenLogType::Info, VERY_LONG_TIME_MS);
}
void ScreenDebug::Warn(const std::string& line)
{
    AddLine(line, ScreenLogType::Warning, VERY_LONG_TIME_MS);
}
void ScreenDebug::Error(const std::string& line)
{
    AddLine(line, ScreenLogType::Error, VERY_LONG_TIME_MS);
}

void ScreenDebug::Draw()
{
    static IFontStyle font;
    font.size = 1.5f;
    font.align = GameFontAlignment::ALIGN_LEFT;

    const float lineHeight = 40.0f;
    const CVector2D startPosition(50.0f, 80.0f);

    const auto isLineVisible = [this](const auto& line)
    {
        if (line.displayTime != -1 && g_timeInMilliseconds - line.timeCreated >= line.displayTime) { return false; }

        if (hideInfoMessages && line.type == ScreenLogType::Info) { return false; }

        return true;
    };

    int visibleLines = 0;
    int startIndex = static_cast<int>(_lines.size()) - 1;

    for (; startIndex >= 0; startIndex--)
    {
        const auto& line = _lines[startIndex];

        if (!isLineVisible(line)) { continue; }

        visibleLines++;

        if (visibleLines >= _maxDrawLines) { break; }
    }

    int drawIndex = 0;

    for (int i = startIndex + 1; i < static_cast<int>(_lines.size()); i++)
    {
        const auto& line = _lines[i];

        if (!isLineVisible(line)) { continue; }

        font.color = COLOR_WHITE;

        if (line.type == ScreenLogType::Error) { font.color = CRGBA(255, 0, 0); }
        else if (line.type == ScreenLogType::Warning) { font.color = CRGBA(255, 255, 0); }

        CVector2D position(startPosition.x, startPosition.y + (drawIndex * lineHeight));

        DrawUtils::DrawText(line.text, position, font);

        drawIndex++;
    }
}

void ScreenDebug::Clear()
{
    _lines.clear();
}