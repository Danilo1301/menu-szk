#pragma once

#include "aml-psdk/game_sa/utils/OpcodeCallerIDs.h"
#include "aml-psdk/game_sa/utils/OpcodeCaller_test.h"

#include "../utils/drawUtils.h"
#include "aml-psdk/gta_base/Vector.h"
#include "menu/menu.h"
#include "mod/logger.h"
#include "webPlayer.h"
#include <map>

#include "../container/container.h"
#include "../menuSZK.h"

/*

lembrar de remover refs dos peds

*/

inline bool LoadModel(int modelId)
{
    logger->Info("Finding if model %d is loaded", modelId);

    if (Command<Commands::HAS_MODEL_LOADED>(modelId))
    {
        return true;
    }

    logger->Info("Requesting model %d", modelId);

    Command<Commands::REQUEST_MODEL>(modelId);
    Command<Commands::LOAD_ALL_MODELS_NOW>();

    if (!Command<Commands::HAS_MODEL_LOADED>(modelId))
    {

        logger->Info("model nao carregado, but whatever :P");
        return false;
    }

    logger->Info("DIZ QUE FOI CARREGADO");
    return true;
}

struct SyncedPlayer
{
    int handle;
    CPed *ped;
    WebPlayer webPlayer;
    unsigned int lastPositionUpdate = 0;
};

inline std::map<std::string, SyncedPlayer> _syncedPlayers;

inline int CreatePlayerPed(WebPlayer &player, int modelId)
{
    int handle = 0;

    LoadModel(modelId);

    logger->Info("Create char");

    Command<Commands::CREATE_CHAR>(4, modelId, player.x, player.y, player.z, &handle);

    return handle;
}

inline void SyncPlayers(std::map<std::string, WebPlayer> *players)
{
    // runs on gameProcessEvent

    auto now = g_timeInMilliseconds;

    int pedModelId = 280;

    for (auto &[id, player] : *players)
    {
        auto it = _syncedPlayers.find(id);

        if (it == _syncedPlayers.end())
        {
            logger->Info("Creating a new ped");

            int handle = CreatePlayerPed(player, pedModelId);

            _syncedPlayers[id] = {handle, nullptr};

            continue;
        }

        SyncedPlayer &syncedPlayer = it->second;
        syncedPlayer.webPlayer = player;

        if (!Command<Commands::DOES_CHAR_EXIST>(syncedPlayer.handle))
        {
            logger->Info("what, it doesnt exist anymore");

            syncedPlayer.handle = CreatePlayerPed(player, pedModelId);
            continue;
        }

        if (now - syncedPlayer.lastPositionUpdate >= 1000)
        {
            float currentX;
            float currentY;
            float currentZ;

            Command<Commands::GET_CHAR_COORDINATES>(syncedPlayer.handle, &currentX, &currentY, &currentZ);

            float dx = currentX - player.x;
            float dy = currentY - player.y;
            float dz = currentZ - player.z;

            float distance = sqrtf(dx * dx + dy * dy + dz * dz);

            Command<Commands::SET_CHAR_COORDINATES>(syncedPlayer.handle, player.x, player.y, player.z);

            // if (distance > 50.0f)
            // {
            //     Command<Commands::SET_CHAR_COORDINATES>(
            //         syncedPlayer.handle,
            //         player.x,
            //         player.y,
            //         player.z
            //     );
            // }
            // else
            // {
            //     Command<0x0603>(
            //         syncedPlayer.handle,
            //         &player.x,
            //         &player.y,
            //         &player.z,
            //         6,
            //         false
            //     );
            // }

            syncedPlayer.lastPositionUpdate = now;
        }
    }
}

inline void DrawPlayersTag()
{
    IFont font;
    font.size = 3.0f;

    for (auto p : _syncedPlayers)
    {
        auto webPlayer = p.second.webPlayer;

        CVector worldPosition;

        Command<Commands::GET_CHAR_COORDINATES>(p.second.handle, &worldPosition.x, &worldPosition.y, &worldPosition.z);

        CVector screenPosition;
        float scaleX;
        float scaleY;

        if (!CalcScreenCoors(worldPosition, screenPosition))
            continue;

        screenPosition.x = DrawUtils::MapWidthFromOS(screenPosition.x);
        screenPosition.y = DrawUtils::MapHeightFromOS(screenPosition.y);

        CVector2D screenPos(screenPosition.x, screenPosition.y);

        DrawUtils::DrawText(webPlayer.name, screenPos, font, CVector2D(1, 1), false, 1);
    }
}