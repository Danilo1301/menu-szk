#pragma once

#include "aml-psdk/gta_base/Vector.h"
#include "audio/audio.h"
#include "keyboard/keyboard.h"
#include "mod/logger.h"
#include "pch.h"

#include "aml-psdk/gta_base/RGBA.h"
#include "cellphone/cellphone.h"
#include "menuSZK/imenuSZK.h"
#include "peds.h"
#include "radarBlip/radarBlip.h"
#include "src/container/container.h"
#include "src/infoMessage/infoMessage.h"
#include "logHelper.h"
#include "src/screenDebug/screenDebug.h"
#include "src/utils/textureLoader.h"
#include "textures/texture.h"
#include "utils/drawUtils.h"
#include "localization/localization.h"
#include "vehicles.h"
#include "widget/widget.h"
#include "window/windowManager.h"
#include <cstddef>
#include <functional>
#include <string>

class MenuSZK : public IMenuSZK
{
public:
    MenuSZK();

    IWindow* CreateWindow(float x, float y, float width, const std::string& title, const std::string& subtitle) override
    {
        auto window = WindowManager::CreateWindow(x, y, title, subtitle, width);
        return (IWindow*)window;
    }

    void AddCellphoneScript(const std::string& text, const std::string& iconPath, std::function<void()> fn) override
    {
        // ScriptsPanel::AddScriptFunction(text, fn);
        Cellphone::ScriptsCellphone->AddItem(text, iconPath, fn);
    }

    std::vector<GameEntity> GetPeds() override
    {
        return Peds::GetPeds();
    }
    std::vector<GameEntity> GetVehicles() override
    {
        return Vehicles::GetVehicles();
    }

    IWidget* CreateWidget(float x, float y, float size, const std::string& bgImage, const std::string& image) override
    {
        auto widget = Widget::CreateWidget(x, y, size, bgImage, image);

        return (IWidget*)widget;
    }

    RwTexture* LoadTexture(const std::string& pngFilePath, bool cache = true) override
    {
        RwTexture* ptr = nullptr;

        if (cache) { ptr = LoadRwTextureFromFileAndCache(pngFilePath, "texture", false, COLOR_WHITE); }
        else
        {
            ptr = LoadRwTextureFromFile(pngFilePath, "texture", false, COLOR_WHITE);
        }

        return ptr;
    }

    ITexture* GetOrLoadTexture(const std::string& pngFilePath) override
    {
        auto texture = new Texture(pngFilePath, "texture", false, COLOR_WHITE);
        return texture;
    }

    IContainer* GetMainContainer() override
    {
        return (IContainer*)Container::MainContainer;
    }

    void DrawText(const std::string& text, CVector2D& position, IFontStyle& style) override
    {
        DrawUtils::DrawText(text, position, style);
    }

    void DrawRect(CVector2D position, CVector2D size, CRGBA color) override
    {
        DrawUtils::DrawRect(position, size, color);
    }

    void DrawTexture(ITexture* texture, CVector2D position, CVector2D size, CRGBA color) override
    {
        DrawUtils::DrawTexture((Texture*)texture, position, size, color);
    }

    IRadarBlip* CreateBlip(ITexture* texture, CVector worldPosition, float size) override
    {
        auto blip = new RadarBlip();
        blip->texture = texture;
        blip->size = size;
        blip->worldPosition = worldPosition;

        return (IRadarBlip*)blip;
    }

    void DrawTextureOnRadar(ITexture* texture, CVector2D radarPosition, CRGBA color, CVector2D size) override
    {
        DrawUtils::DrawTextureOnRadar((Texture*)texture, radarPosition, size, color);
    }

    CVector2D ConvertWorldToScreenCoords(CVector worldPosition, bool useMenuCoords) override
    {
        return DrawUtils::ConvertWorldToScreenCoords(worldPosition, useMenuCoords);
    }

    CVector2D ConvertMenuScreenCoordsToOS(CVector2D menuCoords) override
    {
        CVector2D result;

        result.x = DrawUtils::MapHeightToOS(menuCoords.x);
        result.y = DrawUtils::MapWidthToOS(menuCoords.y);

        return result;
    }

    CVector2D ConvertOS_ScreenCordsToMenu(CVector osCoords) override
    {
        CVector2D result;

        result.x = DrawUtils::MapHeightFromOS(osCoords.x);
        result.y = DrawUtils::MapWidthFromOS(osCoords.y);

        return result;
    }

    IAudio* GetOrLoadAudio(const std::string& audioFilePath) override
    {
        auto audio = new Audio(audioFilePath, false);
        return (IAudio*)audio;
    }

    IAudio* GetOrLoad3DAudio(const std::string& audioFilePath) override
    {
        auto audio = new Audio(audioFilePath, true);
        return audio;
    }

    std::string GetLocalizationText(const std::string& key) override
    {
        return Localization::Get(key);
    }

    void LogOnScreen(const std::string& line, ScreenLogType type, int displayTime) override
    {
        ScreenDebug::Main->AddLine(line, type, displayTime);
    }

    void ShowBottomMessage(const std::string& line, int duration = 5000) override
    {
        InfoMessage::GetBottom()->SetMessage(line, duration);
    }

    void BeginOperation(unsigned int operationCode, const std::string& operationName, const std::string& description) override
    {
        LogHelper::BeginOperation(operationCode, operationName, description);
    }

    void EndOperation(unsigned int operationCode, const std::string& result = "") override
    {
        LogHelper::EndOperation(operationCode, result);
    }

    void AddLogMessage(const std::string& message) override
    {
        LogHelper::OnLoggerMessage(eLogPrio::LogP_Info, message);
    }

    void ToggleKeyboard(const std::string& initialValue, KeyboardFlags flags) override
    {
        if (Keyboard::IsVisible()) { Keyboard::Close(); }
        else
        {
            Keyboard::Open(initialValue, flags);
        }
    }
};

extern MenuSZK* menuSZK;

// MenuSZK* menuSZK = new MenuSZK();