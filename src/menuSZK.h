#pragma once

#include "pch.h"

class MenuSZK
{
  public:
    static void OnPreload();
    static void OnLoad();

    static void OnTimerUpdate();
    static void OnGameProcess();
    static void OnRender();

  private:
    static void SetupOnPlayerReady();
};