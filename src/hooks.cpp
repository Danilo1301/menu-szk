#include "hooks.h"

#include "aml-psdk/game_sa/Events.h"
#include "aml-psdk/game_sa/base/Timer.h"
#include "audio/soundSystem/CSoundSystem.h"
#include "logHelper.h"
#include "menuInterface.h"
#include "mod/logger.h"

#include "input.h"
#include "menuSZK.h"
#include "pch.h"
#include "src/screenDebug/screenDebug.h"
#include "webServer/webServer.h"
#include "window/slider.h"

#include "menus/introductionImage.h"

#include <chrono>
#include <sys/stat.h>

DECL_HOOKv(CTimer__Update)
{
    LogHelper::SetFrameOperation("CTimer update [1]");

    CTimer__Update();

    auto prevTime = g_timeInMilliseconds;
    auto timerNow = CTimer::m_snTimeInMilliseconds;
    auto now = timerNow == 0 ? prevTime + 10 : timerNow;
    auto dt = now - prevTime;

    g_timeInMilliseconds = now;
    g_deltaTime = dt;

    LogHelper::SetFrameOperation("CTimer update [2]");

    MenuSZK::OnTimerUpdate();

    LogHelper::SetFrameOperation("CTimer update [3]");

    if (!g_gameHasFirstProcessed)
    {
        menuInterface->onGameProcess->Emit(dt);
    }

    LogHelper::SetFrameOperation("CTimer update [4]");

    WebServer::OnUpdate(g_timeInMilliseconds);

    LogHelper::SetFrameOperation("CTimer update [end]");
}

DECL_HOOK(void *, CGame__Process)
{
    LogHelper::SetFrameOperation("CGame Process [1]");

    if (!g_gameHasFirstProcessed)
    {
        g_gameHasFirstProcessed = true;
        ScreenDebug::Main->Clear();
    }

    //

    static unsigned int lastTime = CTimer::m_snTimeInMilliseconds;
    unsigned int now = CTimer::m_snTimeInMilliseconds;
    int deltaTime = std::min((int)(now - lastTime), (int)100);
    lastTime = now;

    //

    void *result = CGame__Process();

    LogHelper::SetFrameOperation("CGame Process [2]");

    //

    if (BASS)
    {
        // logger->Info("Updating soundsys");
        soundsys->Update();
    }

    //

    MenuSZK::OnGameProcess();

    LogHelper::SetFrameOperation("CGame Process [3]");

    menuInterface->onGameProcess->Emit(deltaTime);

    LogHelper::SetFrameOperation("CGame Process [4]");

    menuInterface->onScriptProcess->Emit(deltaTime);

    LogHelper::SetFrameOperation("CGame Process [end]");

    return result;
}

DECL_HOOK(void, PreRenderEnd, void *self)
{
    LogHelper::SetFrameOperation("PreRenderEnd [1]");

    g_framesDrawn++;

    static auto g_lastRenderTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    g_renderDeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_lastRenderTime).count();
    g_lastRenderTime = now;

    PreRenderEnd(self);

    LogHelper::SetFrameOperation("PreRenderEnd [2]");

    MenuSZK::OnRender();

    LogHelper::SetFrameOperation("PreRenderEnd [3]");

    menuInterface->onPreRenderEnd->Emit(g_renderDeltaTime);

    LogHelper::SetFrameOperation("PreRenderEnd [end]");
}

DECL_HOOK(void, TouchEvent, int actionType, int trackNum, int x, int y)
{
    LogHelper::SetFrameOperation("TouchEvent");

    LOG_PER_FRAME("TouchEvent");

    Input::OnTouchEvent(actionType, trackNum, x, y, g_timeInMilliseconds);

    if (Input::NeedsToBeBlocked(x, y))
    {
        // logger->Info("bloqueado");
        LOG_PER_FRAME("TouchEvent [end - blocked]");
        return;
    }

    TouchEvent(actionType, trackNum, x, y);

    LogHelper::SetFrameOperation("TouchEvent end");
}

void DoHooks()
{
    logger->Info("Hooking...");

    void *hGTASA = dlopen("libGTASA.so", RTLD_LAZY);
    uintptr_t pGTASA = aml->GetLib("libGTASA.so");

    SET_TO(pPedPool, aml->GetSym(hGTASA, "_ZN6CPools11ms_pPedPoolE"));
    SET_TO(userPaused, aml->GetSym(hGTASA, "_ZN6CTimer11m_UserPauseE"));
    SET_TO(codePaused, aml->GetSym(hGTASA, "_ZN6CTimer11m_CodePauseE"));
    SET_TO(camera, aml->GetSym(hGTASA, "TheCamera"));

    SET_TO(GetPedRef, aml->GetSym(hGTASA, "_ZN6CPools9GetPedRefEP4CPed"));
    SET_TO(OS_ScreenGetWidth, aml->GetSym(hGTASA, "_Z17OS_ScreenGetWidthv"));
    SET_TO(OS_ScreenGetHeight, aml->GetSym(hGTASA, "_Z18OS_ScreenGetHeightv"));
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

    HOOK(CTimer__Update, CTimer::Update); // just an example!
    HOOK(CGame__Process, aml->GetSym(hGTASA, "_ZN5CGame7ProcessEv"));
    HOOK(PreRenderEnd, aml->GetSym(hGTASA, "_ZN6CDebug22DebugDisplayTextBufferEv"));
    HOOK(TouchEvent, aml->GetSym(hGTASA, "_Z14AND_TouchEventiiii"));

    logger->Info("Hooks ok");
}