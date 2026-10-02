#pragma once

#include "pch.h"

class Mod
{
public:
    static void OnPreload();
    static void OnLoad();

    static void OnTimerUpdate();
    static void OnModProcess();
    static void OnRender();

private:
    static void SetupOnPlayerReady();
};