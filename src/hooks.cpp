#include "hooks.h"

#include "aml-psdk/game_sa/Events.h"
#include "aml-psdk/game_sa/base/Timer.h"
#include "menuInterface.h"
#include "mod/logger.h"

#include "input.h"
#include "menuSZK.h"
#include "pch.h"
#include "webServer/webServer.h"
#include "window/slider.h"

#include <chrono>
#include <sys/stat.h>

bool _createdMenu = false;

DECL_HOOKv(CTimer__Update)
{
    LOG_PER_FRAME("OnTimerUpdate");

    CTimer__Update();

    auto prevTime = g_timeInMilliseconds;
    auto timerNow = CTimer::m_snTimeInMilliseconds;

    auto now = timerNow == 0 ? prevTime + 10 : timerNow;

    auto dt = now - prevTime;

    g_timeInMilliseconds = now;
    g_deltaTime = dt;

    MenuSZK::OnTimerUpdate();

    if (!g_gameHasFirstProcessed)
    {
        LOG_PER_FRAME("Invoking onGameProcess");
        menuInterface->onGameProcess->Emit(dt);
        LOG_PER_FRAME("onGameProcess end");
    }

    if (g_framesDrawn >= 30 && !_createdMenu)
    {
        _createdMenu = true;

        auto container = Container::MainContainer->AddChild("slider");

        static float testFloat = 3.25f;

        auto slider = new Slider(container, &testFloat, 0, 100, 1);
    }

    WebServer::OnUpdate(g_timeInMilliseconds);

    LOG_PER_FRAME("OnTimerUpdate [end]");
}

DECL_HOOK(void *, CGame__Process)
{
    LOG_PER_FRAME("CGame__Process");

    g_gameHasFirstProcessed = true;

    //

    static unsigned int lastTime = CTimer::m_snTimeInMilliseconds;
    unsigned int now = CTimer::m_snTimeInMilliseconds;
    int deltaTime = std::min((int)(now - lastTime), (int)100);
    lastTime = now;

    //

    void *result = CGame__Process();

    MenuSZK::OnGameProcess();

    LOG_PER_FRAME("Invoking onGameProcess");
    menuInterface->onGameProcess->Emit(deltaTime);
    LOG_PER_FRAME("onGameProcess end");

    LOG_PER_FRAME("CGame__Process [end]");

    return result;
}

DECL_HOOK(void, PreRenderEnd, void *self)
{
    g_framesDrawn++;

    static auto g_lastRenderTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    g_renderDeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_lastRenderTime).count();
    g_lastRenderTime = now;

    LOG_PER_FRAME("PreRenderEnd");

    PreRenderEnd(self);

    MenuSZK::OnRender();

    // logger->Info("Render deltaTime = %lld ms", g_renderDeltaTime);

    menuInterface->onPreRenderEnd->Emit(g_renderDeltaTime);

    //

    LOG_PER_FRAME("PreRenderEnd [end]");
}

DECL_HOOK(void, TouchEvent, int actionType, int trackNum, int x, int y)
{
    LOG_PER_FRAME("TouchEvent");

    Input::OnTouchEvent(actionType, trackNum, x, y, g_timeInMilliseconds);

    if (Input::NeedsToBeBlocked(x, y))
    {
        // logger->Info("bloqueado");
        LOG_PER_FRAME("TouchEvent [end - blocked]");
        return;
    }

    TouchEvent(actionType, trackNum, x, y);

    LOG_PER_FRAME("TouchEvent [end]");
}

void DoHooks()
{
    logger->Info("Hooking...");

    void *hGTASA = dlopen("libGTASA.so", RTLD_LAZY);
    uintptr_t pGTASA = aml->GetLib("libGTASA.so");

    SET_TO(pPedPool, aml->GetSym(hGTASA, "_ZN6CPools11ms_pPedPoolE"));

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