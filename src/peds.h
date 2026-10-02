#pragma once

#include "menuSZK/imenuSZK.h"
#include "pch.h"
#include "utils/eventListener.h"
#include <vector>

class Peds
{
public:
    static EventListener<GameEntity>* onPedAdded;
    static EventListener<GameEntity>* onPedRemoved;

    static void Initialize();
    static void Process();

    static std::vector<GameEntity>& GetPeds();
};