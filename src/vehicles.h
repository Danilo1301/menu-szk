#pragma once

#include "menuSZK/imenuSZK.h"
#include "pch.h"
#include "utils/eventListener.h"
#include <vector>

class Vehicles
{
public:
    static EventListener<GameEntity>* onVehicleAdded;
    static EventListener<GameEntity>* onVehicleRemoved;

    static void Initialize();
    static void Process();

    static std::vector<GameEntity>& GetVehicles();
};