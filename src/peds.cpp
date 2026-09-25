#include "peds.h"

#include "aml-psdk/game_sa/Events.h"

#include "aml-psdk/game_sa/entity/Ped.h"
#include "aml-psdk/game_sa/other/Pools.h"

#include "menu/menu.h"
#include "utils/eventListener.h"

#include <map>
#include <vector>

std::map<void*, GameEntity> storedPeds;
std::vector<GameEntity> storedPedsVec;

EventListener<GameEntity>* Peds::onPedFound = new EventListener<GameEntity>();
EventListener<GameEntity>* Peds::onPedDestroy = new EventListener<GameEntity>();

void Peds::Initialize()
{
    Events::pedDtorEvent.before += [](CPed* ped)
    {
        auto it = storedPeds.find(ped);

        if(it == storedPeds.end())
            return;

        onPedDestroy->Emit(it->second);

        storedPeds.erase(it);
        
        storedPedsVec.erase(
            std::remove_if(
                storedPedsVec.begin(),
                storedPedsVec.end(),
                [ped](const GameEntity& entity)
                {
                    return entity.ptr == ped;
                }
            ),
            storedPedsVec.end()
        );
    };
}

void Peds::Process()
{
    for(int i = 0; i < CPools::ms_pPedPool->m_nSize; i++)
    {
        CPed* ped = CPools::ms_pPedPool->GetAt(i);

        if(!ped)
            continue;

        if(storedPeds.find(ped) != storedPeds.end())
            continue;

        int ref = CPools::ms_pPedPool->GetRef(ped);

        GameEntity entity;
        // inicializa entity aqui

        storedPeds[ped] = entity;
        storedPedsVec.push_back(entity);

        onPedFound->Emit(entity);

        logger->Info("Ped apareceu: index=%d ped=%p ref=%d", i, ped, ref);
    }
}

std::vector<GameEntity>& Peds::GetPeds()
{
    return storedPedsVec;
}