#include "vehicles.h"

#include "aml-psdk/game_sa/Events.h"

#include "aml-psdk/game_sa/entity/Ped.h"
#include "aml-psdk/game_sa/entity/Vehicle.h"
#include "aml-psdk/game_sa/other/Pools.h"

#include "menu/menu.h"
#include "utils/eventListener.h"

#include <map>
#include <vector>

std::map<void *, GameEntity> storedVehicles;
std::vector<GameEntity> storedVehiclesVec;

EventListener<GameEntity> *Vehicles::onVehicleAdded = new EventListener<GameEntity>();
EventListener<GameEntity> *Vehicles::onVehicleRemoved = new EventListener<GameEntity>();

void Vehicles::Initialize()
{
    Events::vehicleDtorEvent.before += [](CVehicle *veh)
    {
        auto it = storedVehicles.find(veh);

        if (it == storedVehicles.end())
            return;

        logger->Info("Vehicle removed, vehicle=%p ref=%d", it->second.ptr, it->second.ref);

        onVehicleRemoved->Emit(it->second);

        storedVehicles.erase(it);

        storedVehiclesVec.erase(std::remove_if(storedVehiclesVec.begin(), storedVehiclesVec.end(),
                                    [veh](const GameEntity &entity) { return entity.ptr == veh; }),
            storedVehiclesVec.end());
    };
}

void Vehicles::Process()
{
    for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++)
    {
        CVehicle *veh = CPools::ms_pVehiclePool->GetAt(i);

        if (!veh)
            continue;

        if (storedVehicles.find(veh) != storedVehicles.end())
            continue;

        int ref = CPools::ms_pVehiclePool->GetRef(veh);

        GameEntity entity;
        // inicializa entity aqui

        storedVehicles[veh] = entity;
        storedVehiclesVec.push_back(entity);

        logger->Info("Vehicle added: index=%d vehicle=%p ref=%d", i, veh, ref);

        onVehicleAdded->Emit(entity);
    }
}

std::vector<GameEntity> &Vehicles::GetVehicles() { return storedVehiclesVec; }