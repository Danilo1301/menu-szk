#include "vehicles.h"

#include "aml-psdk/game_sa/entity/Vehicle.h"
#include "aml-psdk/game_sa/other/Pools.h"
#include "aml-psdk/game_sa/Events.h"

#include "menuSZK/imenuSZK.h"
#include "utils/eventListener.h"

#include <algorithm>
#include <map>
#include <vector>

std::map<void*, GameEntity> storedVehicles;
std::vector<GameEntity> storedVehiclesVec;

EventListener<GameEntity>* Vehicles::onVehicleAdded = new EventListener<GameEntity>();
EventListener<GameEntity>* Vehicles::onVehicleRemoved = new EventListener<GameEntity>();

void Vehicles::Initialize()
{
    Events::vehicleDtorEvent.before += [](CVehicle* vehicle)
    {
        if (!vehicle) return;

        auto it = storedVehicles.find(vehicle);

        if (it == storedVehicles.end()) return;

        GameEntity entity = it->second;

        logger->Info("Vehicle removed: vehicle=%p ref=%d", entity.ptr, entity.ref);

        onVehicleRemoved->Emit(entity);

        storedVehiclesVec.erase(std::remove_if(storedVehiclesVec.begin(),
                                    storedVehiclesVec.end(),
                                    [vehicle](const GameEntity& storedEntity) { return storedEntity.ptr == vehicle; }),
            storedVehiclesVec.end());

        storedVehicles.erase(it);
    };
}

void Vehicles::Process()
{
    for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++)
    {
        CVehicle* vehicle = CPools::ms_pVehiclePool->GetAt(i);

        if (!vehicle) continue;
        if (storedVehicles.find(vehicle) != storedVehicles.end()) continue;

        int ref = CPools::ms_pVehiclePool->GetRef(vehicle);

        GameEntity entity;
        entity.ptr = vehicle;
        entity.ref = ref;

        storedVehicles[vehicle] = entity;
        storedVehiclesVec.push_back(entity);

        logger->Info("Vehicle added: index=%d vehicle=%p ref=%d", i, vehicle, ref);

        onVehicleAdded->Emit(entity);
    }
}

std::vector<GameEntity>& Vehicles::GetVehicles()
{
    return storedVehiclesVec;
}