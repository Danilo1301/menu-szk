#include "hooks.h"

#include "aml-psdk/game_sa/base/Timer.h"
#include "audio/soundSystem/CSoundSystem.h"
#include "config.h"
#include "input_old.h"
#include "logHelper.h"
#include "menuSZK.h"
#include "menuOperation.h"
#include "menuSZK/imenuSZK.h"
#include "mod/logger.h"
#include "logHelper.h"

#include "input.h"
#include "mod.h"
#include "pch.h"
#include "radarBlip/radarBlip.h"
#include "src/logHelper.h"
#include "src/screenDebug/screenDebug.h"
#include "webServer/webServer.h"
#include "menus/menuSettings.h"

#include <chrono>
#include <cstdlib>
#include <sys/stat.h>

DECL_HOOKv(CTimer__Update)
{
    BEGIN_OPERATION(op_TimerUpdate);

    CTimer__Update();

    auto prevTime = g_timeInMilliseconds;
    auto timerNow = CTimer::m_snTimeInMilliseconds;
    auto now = timerNow == 0 ? prevTime + 10 : timerNow;
    auto dt = now - prevTime;

    g_timeInMilliseconds = now;
    g_deltaTime = dt;

    if (use_old_input_system()) { Input_old::Update(); }

    Input::ProcessTouchEvents();

    Mod::OnTimerUpdate();

    BEGIN_OPERATION(op_ProcessMenuEvents);

    if (!g_gameHasFirstProcessed) { menuSZK->onMenuProcess->Emit(dt); }

    END_OPERATION(op_ProcessMenuEvents);

    WebServer::OnUpdate(g_timeInMilliseconds);

    END_OPERATION(op_TimerUpdate);
}

// DECL_HOOK(void*, CGame__Process)
// {
//     BEGIN_OPERATION(op_GameProcces);

//     if (!g_gameHasFirstProcessed)
//     {
//         g_gameHasFirstProcessed = true;
//         ScreenDebug::Main->Clear();
//     }

//     //

//     void* result = CGame__Process();

//     //

//     if (BASS)
//     {
//         // logger->Info("Updating soundsys");
//         soundsys->Update();
//     }

//     //

//     Mod::OnModProcess();

//     static unsigned int lastTime = CTimer::m_snTimeInMilliseconds;
//     unsigned int now = CTimer::m_snTimeInMilliseconds;
//     int deltaTime = std::min((int)(now - lastTime), (int)100);
//     lastTime = now;

//     BEGIN_OPERATION(op_ProcessMenuEvents);
//     menuSZK->onMenuProcess->Emit(deltaTime);
//     END_OPERATION(op_ProcessMenuEvents);

//     END_OPERATION(op_GameProcces);

//     return result;
// }

DECL_HOOK(void, PreRenderEnd, void* self)
{
    BEGIN_OPERATION(op_PreRenderEnd);

    g_framesDrawn++;

    static auto g_lastRenderTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    g_renderDeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_lastRenderTime).count();
    g_lastRenderTime = now;

    PreRenderEnd(self);

    BEGIN_OPERATION_DESC(op_Test, "before menu");
    menuSZK->onDrawBeforeMenu->Emit(g_renderDeltaTime);
    END_OPERATION(op_Test);

    BEGIN_OPERATION_DESC(op_Test, "mod on render");
    Mod::OnRender();
    END_OPERATION(op_Test);

    BEGIN_OPERATION_DESC(op_Test, "after menu");
    menuSZK->onDrawAfterMenu->Emit(g_renderDeltaTime);
    END_OPERATION(op_Test);

    END_OPERATION(op_PreRenderEnd);
}

DECL_HOOK(void, TouchEvent, int actionType, int trackNum, int x, int y)
{
    BEGIN_OPERATION(op_TouchEvent);

    static bool _isFirstTest = true;
    static std::string _inputFilePath = GetMenuAssetPath("TOUCH_TEST");

    if (_isFirstTest)
    {
        //
        JustCreateFile(_inputFilePath);
    }

    if (LogHelper::bDuringTouchEvent)
    {
        logger->Error("bDuringTouchEvent is true during start of the event");
        std::abort();
        return;
    }

    LogHelper::bDuringTouchEvent = true;

    Input::OnTouchEvent(actionType, trackNum, x, y, g_timeInMilliseconds);

    if (Input::NeedsToBeBlocked(x, y))
    {
        LogHelper::bDuringTouchEvent = false;

        //TouchEvent(actionType, trackNum, 0, 0);

        END_OPERATION_RESULT(op_TouchEvent, "blocked");

        return;
    }

    LogHelper::bDuringTouchEvent = false;

    TouchEvent(actionType, trackNum, x, y);

    if (_isFirstTest)
    {
        _isFirstTest = false;
        RemoveFile(_inputFilePath);

        ScreenDebug::Main->AddLine("Input is working!", ScreenLogType::Special, 3000);
    }

    END_OPERATION(op_TouchEvent);
}

DECL_HOOKv(DrawRadarGangOverlay, bool b)
{
    BEGIN_OPERATION(op_DrawRadarGangOverlay);

    DrawRadarGangOverlay(b);

    RadarBlip::DrawAll();

    menuSZK->onPostDrawRadar->Emit();

    END_OPERATION(op_DrawRadarGangOverlay);
}

void DoHooks()
{
    logger->Info("Hooking...");

    void* hGTASA = dlopen("libGTASA.so", RTLD_LAZY);
    uintptr_t pGTASA = aml->GetLib("libGTASA.so");

    SET_TO(pPedPool, aml->GetSym(hGTASA, "_ZN6CPools11ms_pPedPoolE"));
    SET_TO(userPaused, aml->GetSym(hGTASA, "_ZN6CTimer11m_UserPauseE"));
    SET_TO(codePaused, aml->GetSym(hGTASA, "_ZN6CTimer11m_CodePauseE"));
    SET_TO(camera, aml->GetSym(hGTASA, "TheCamera"));
    SET_TO(m_pWidgets, *(void**)(pGTASA + BYBIT(0x67947C, 0x850910)));

    SET_TO(GetPedRef, aml->GetSym(hGTASA, "_ZN6CPools9GetPedRefEP4CPed"));
    SET_TO(CSprite2d_DrawRect, aml->GetSym(hGTASA, "_ZN9CSprite2d8DrawRectERK5CRectRK5CRGBA"));
    SET_TO(FontSetOrientation, aml->GetSym(hGTASA, "_ZN5CFont14SetOrientationEh"));
    SET_TO(FontSetColor, aml->GetSym(hGTASA, "_ZN5CFont8SetColorE5CRGBA"));
    SET_TO(FontSetBackground, aml->GetSym(hGTASA, "_ZN5CFont13SetBackgroundEhh"));
    SET_TO(FontSetWrapx, aml->GetSym(hGTASA, "_ZN5CFont8SetWrapxEf"));
    SET_TO(FontSetStyle, aml->GetSym(hGTASA, "_ZN5CFont12SetFontStyleEh"));
    SET_TO(FontSetScale, aml->GetSym(hGTASA, "_ZN5CFont8SetScaleEf"));
    SET_TO(FontSetEdge, aml->GetSym(hGTASA, "_ZN5CFont7SetEdgeEa"));
    SET_TO(FontSetProportional, aml->GetSym(hGTASA, "_ZN5CFont15SetProportionalEh"));
    SET_TO(FontSetDropShadowPosition, aml->GetSym(hGTASA, "_ZN5CFont21SetDropShadowPositionEa"));
    SET_TO(FontSetDropColor, aml->GetSym(hGTASA, "_ZN5CFont12SetDropColorE5CRGBA"));
    SET_TO(FontPrintString, aml->GetSym(hGTASA, "_ZN5CFont11PrintStringEffPt"));
    SET_TO(AsciiToGxtChar, aml->GetSym(hGTASA, "_Z14AsciiToGxtCharPKcPt"));
    SET_TO(RenderFontBuffer, aml->GetSym(hGTASA, "_ZN5CFont16RenderFontBufferEv"));
    SET_TO(CSprite2d_DrawSprite, aml->GetSym(hGTASA, "_ZN9CSprite2d4DrawERK5CRectRK5CRGBA"));
    SET_TO(DisplayThisBlip, aml->GetSym(hGTASA, "_ZN6CRadar15DisplayThisBlipEia"));
    SET_TO(TransformRealWorldPointToRadarSpace, aml->GetSym(hGTASA, "_ZN6CRadar35TransformRealWorldPointToRadarSpaceER9CVector2DRKS0_"));
    SET_TO(LimitRadarPoint, aml->GetSym(hGTASA, "_ZN6CRadar15LimitRadarPointER9CVector2D"));
    SET_TO(TransformRadarPointToScreenSpace, aml->GetSym(hGTASA, "_ZN6CRadar32TransformRadarPointToScreenSpaceER9CVector2DRKS0_"));
    SET_TO(CSprite_CalcScreenCoors, aml->GetSym(hGTASA, "_ZN7CSprite15CalcScreenCoorsERK5RwV3dPS0_PfS4_bb"));
    SET_TO(CTouchInterface_m_bTouchDown, aml->GetSym(hGTASA, "_ZN15CTouchInterface12m_bTouchDownE"));
    SET_TO(m_vecCachedPos, aml->GetSym(hGTASA, "_ZN15CTouchInterface14m_vecCachedPosE"));

    // this could be the best place to draw blips.. (for 32)
    // HOOKPLT(RadarBlipsDraw, pGTASA + 0x66E910);

    HOOK(CTimer__Update, CTimer::Update); // just an example!
    //HOOK(CGame__Process, aml->GetSym(hGTASA, "_ZN5CGame7ProcessEv"));
    HOOK(PreRenderEnd, aml->GetSym(hGTASA, "_ZN6CDebug22DebugDisplayTextBufferEv"));

    if (use_old_input_system() == false) { HOOK(TouchEvent, aml->GetSym(hGTASA, "_Z14AND_TouchEventiiii")); }

    HOOK(DrawRadarGangOverlay, aml->GetSym(hGTASA, "_ZN6CRadar20DrawRadarGangOverlayEb"));

    Events::gameProcessEvent.after += []()
    {
        BEGIN_OPERATION(op_GameProcces);

        if (!g_gameHasFirstProcessed)
        {
            g_gameHasFirstProcessed = true;
            ScreenDebug::Main->Clear();
        }

        //

        if (BASS)
        {
            // logger->Info("Updating soundsys");
            soundsys->Update();
        }

        //

        Mod::OnModProcess();

        static unsigned int lastTime = CTimer::m_snTimeInMilliseconds;
        unsigned int now = CTimer::m_snTimeInMilliseconds;
        int deltaTime = std::min((int)(now - lastTime), (int)100);
        lastTime = now;

        BEGIN_OPERATION(op_ProcessMenuEvents);
        menuSZK->onMenuProcess->Emit(deltaTime);
        END_OPERATION(op_ProcessMenuEvents);

        END_OPERATION(op_GameProcces);
    };

    Events::processScriptsEvent += []()
    {
        static unsigned int lastTime = CTimer::m_snTimeInMilliseconds;
        unsigned int now = CTimer::m_snTimeInMilliseconds;
        int deltaTime = std::min((int)(now - lastTime), (int)100);
        lastTime = now;

        BEGIN_OPERATION(op_ProcessScriptEvents);
        menuSZK->onScriptProcess->Emit(deltaTime);
        END_OPERATION(op_ProcessScriptEvents);
    };

    logger->Info("Hooks ok");
}