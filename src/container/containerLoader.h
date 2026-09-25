#pragma once

#include "../config.h"
#include "container.h"
#include "mod/logger.h"
#include "json/json.h"
#include <fstream>
#include <string>

static CRGBA ParseColor(const std::string &value)
{
    if (value.empty() || value[0] != '#')
        return CRGBA(255, 255, 255, 255);

    unsigned int color = 0;

    std::stringstream ss;
    ss << std::hex << value.substr(1);
    ss >> color;

    if (value.length() == 7)
    {
        return CRGBA((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
    }

    return CRGBA((color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

static HorizontalAlign ParseHorizontalAlign(const std::string &value)
{
    if (value == "Middle")
        return HorizontalAlign::Middle;

    if (value == "Right")
        return HorizontalAlign::Right;

    return HorizontalAlign::Left;
}

static VerticalAlign ParseVerticalAlign(const std::string &value)
{
    if (value == "Middle")
        return VerticalAlign::Middle;

    if (value == "Bottom")
        return VerticalAlign::Bottom;

    return VerticalAlign::Top;
}

static MenuFontAlignment ParseFontAlignment(const std::string &value)
{
    if (value == "Center")
        return MenuFontAlignment::ALIGN_CENTER;

    if (value == "Right")
        return MenuFontAlignment::ALIGN_RIGHT;

    return MenuFontAlignment::ALIGN_LEFT;
}

static void LoadContainerFromJSON(Container *container, const Json::Value &data)
{
    if (container == nullptr)
        return;

    container->tag = data.get("tag", "").asString();

    if (data.isMember("style"))
    {
        const Json::Value &style = data["style"];

        container->style.position = style.get("position", "static").asString();

        container->style.left = style.get("left", "0px").asString();

        container->style.right = style.get("right", "auto").asString();

        container->style.top = style.get("top", "0px").asString();

        container->style.bottom = style.get("bottom", "auto").asString();

        container->style.width = style.get("width", "200px").asString();

        container->style.height = style.get("height", "200px").asString();

        if (style.isMember("scale"))
        {
            container->style.scale = CVector2D(style["scale"][0].asFloat(), style["scale"][1].asFloat());
        }

        container->style.opacity = style.get("opacity", 1.0f).asFloat();

        if (style.isMember("margin"))
        {
            const Json::Value &margin = style["margin"];

            container->style.margin.top = margin.get("top", 0.0f).asFloat();

            container->style.margin.left = margin.get("left", 0.0f).asFloat();

            container->style.margin.bottom = margin.get("bottom", 0.0f).asFloat();

            container->style.margin.right = margin.get("right", 0.0f).asFloat();
        }

        container->style.backgroundColor = ParseColor(style.get("backgroundColor", "#ffffffff").asString());

        // container->style.display = style.get("display", "block").asString();

        if (style.isMember("transformOrigin"))
        {
            container->style.transformOrigin =
                CVector2D(style["transformOrigin"][0].asFloat(), style["transformOrigin"][1].asFloat());
        }

        container->style.textHorizontalAlign =
            ParseHorizontalAlign(style.get("textHorizontalAlign", "Left").asString());

        container->style.textVerticalAlign = ParseVerticalAlign(style.get("textVerticalAlign", "Middle").asString());

        if (style.isMember("textOffset"))
        {
            container->style.textOffset = CVector2D(style["textOffset"][0].asFloat(), style["textOffset"][1].asFloat());
        }

        if (style.isMember("backgroundImage"))
        {
            const std::string backgroundImage = style["backgroundImage"].asString();

            if (!backgroundImage.empty())
            {
                const std::string pngFile = GetMenuAssetPath(backgroundImage);

                container->style.backgroundImage = pngFile;
            }
        }
    }

    container->text = data.get("text", "").asString();

    if (data.isMember("font"))
    {
        const Json::Value &font = data["font"];

        container->textFont.size = font.get("size", 1.0f).asFloat();

        container->textFont.color = ParseColor(font.get("color", "#ffffffff").asString());

        container->textFont.align = ParseFontAlignment(font.get("align", "Left").asString());
    }

    if (data.isMember("children"))
    {
        const Json::Value &children = data["children"];

        for (Json::ArrayIndex i = 0; i < children.size(); i++)
        {
            Container *child = Container::CreateContainer("");

            LoadContainerFromJSON(child, children[i]);

            container->AddChild(child);
        }
    }
}

static void LoadContainerFromFile(Container *container, const std::string &filePath)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        logger->Error("Could not open file %s", filePath.c_str());
        return;
    }

    Json::Value data;
    Json::Reader reader;

    if (!reader.parse(file, data))
    {
        logger->Error("Could not parse json from file %s", filePath.c_str());
        return;
    }

    LoadContainerFromJSON(container, data);
}