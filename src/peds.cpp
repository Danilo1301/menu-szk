#include "peds.h"

#include "aml-psdk/game_sa/entity/Ped.h"
#include "aml-psdk/game_sa/other/Pools.h"
#include "aml-psdk/game_sa/Events.h"

#include "menuSZK/imenuSZK.h"
#include "utils/eventListener.h"

#include <algorithm>
#include <map>
#include <vector>

std::map<void*, GameEntity> storedPeds;
std::vector<GameEntity> storedPedsVec;

EventListener<GameEntity>* Peds::onPedAdded = new EventListener<GameEntity>();
EventListener<GameEntity>* Peds::onPedRemoved = new EventListener<GameEntity>();

void Peds::Initialize()
{
    Events::pedDtorEvent.before += [](CPed* ped)
    {
        if (!ped) return;

        auto it = storedPeds.find(ped);

        if (it == storedPeds.end()) return;

        GameEntity entity = it->second;

        logger->Info("Ped removed: ped=%p ref=%d", entity.ptr, entity.ref);

        onPedRemoved->Emit(entity);

        storedPedsVec.erase(
            std::remove_if(
                storedPedsVec.begin(), storedPedsVec.end(), [ped](const GameEntity& storedEntity) { return storedEntity.ptr == ped; }),
            storedPedsVec.end());

        storedPeds.erase(it);
    };
}

void Peds::Process()
{
    for (int i = 0; i < CPools::ms_pPedPool->m_nSize; i++)
    {
        CPed* ped = CPools::ms_pPedPool->GetAt(i);

        if (!ped) continue;
        if (storedPeds.find(ped) != storedPeds.end()) continue;

        int ref = CPools::ms_pPedPool->GetRef(ped);

        GameEntity entity;
        entity.ptr = ped;
        entity.ref = ref;

        storedPeds[ped] = entity;
        storedPedsVec.push_back(entity);

        logger->Info("Ped added: index=%d ped=%p ref=%d", i, ped, ref);

        onPedAdded->Emit(entity);
    }
}

std::vector<GameEntity>& Peds::GetPeds()
{
    return storedPedsVec;
}