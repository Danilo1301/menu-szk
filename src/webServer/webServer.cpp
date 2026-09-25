#include "webServer.h"

#include "aml-psdk/game_sa/Events.h"
#include "aml-psdk/game_sa/utils/OpcodeCallerIDs.h"
#include "aml-psdk/gta_base/Vector.h"
#include "mod/logger.h"
#include "../utils/utils.h"

#include "aml-psdk/game_sa/utils/OpcodeCaller_test.h"
//#include "aml-psdk/game_sa/utils/OpcodeCaller.h" //doesnt work

#include "json/json.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <fstream>

#include "syncPlayers.h"

std::string WebServer::BaseURL = "";
CVector WebServer::PlayerPosition = CVector(0, 0, 0);

unsigned int WebServer::_lastHandshakeTime;

std::map<std::string, WebPlayer> WebServer::_players;

std::string _secretId = "";

std::mutex _playersMutex;
bool _handshakeRunning = false;

void WebServer::Initialze()
{
    //

    std::string nickname;

    {
        std::ifstream file(GetMenuFolder() + "/nickname.txt");

        if (file.is_open())
        {
            std::getline(file, nickname);
            file.close();
        }
    }

    Events::drawHudEvent += []() {
        DrawPlayersTag();
    };

    Events::gameProcessEvent += []() {

        g_logOpcodes = false;

        if(!Command<Commands::IS_PLAYER_PLAYING>(0))
        {
            return;
        }

        int playerActor;

        bool playerResult = Command<Commands::GET_PLAYER_CHAR>(0, &playerActor);

        if(playerActor == -1)
        {
            logger->Error("Test: invalid player actor");
            return;
        }

        float x, y, z;
        Command<Commands::GET_CHAR_COORDINATES>(playerActor, &x, &y, &z);

        PlayerPosition = CVector(x, y, z);

        SyncPlayers(&_players);
    };

    //

    const std::string configPath = "/storage/emulated/0/GTASA_Config/config";
    const std::string secretPath = configPath + "/secret";

    std::ifstream file(secretPath);

    if (file.is_open())
    {
        std::string obfuscatedSecret;
        std::getline(file, obfuscatedSecret);
        file.close();

        _secretId = DeobfuscateSecret(obfuscatedSecret);
    }

    std::string query = "/joined?secretId=" + _secretId + "&name=" + nickname;
    std::string result = DownloadAndGetContent(BaseURL + query);

    if (result.empty())
    {
        logger->Info("Failed to join: empty response");
        return;
    }

    logger->Info("joinedResult: %s", result.c_str());

    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(result, root))
    {
        logger->Info("Failed to parse joined response");
        return;
    }

    if (root.get("error", true).asBool())
    {
        logger->Info("Join failed");
        return;
    }

    _secretId = root.get("secretId", "").asString();

    if (_secretId.empty())
    {
        logger->Info("Server didn't return a secretId");
        return;
    }

    logger->Info("Received secretId: %s", _secretId.c_str());

    mkdir("/storage/emulated/0/GTASA_Config", 0777);
    mkdir(configPath.c_str(), 0777);

    std::ofstream output(secretPath);

    if (output.is_open())
    {
        output << ObfuscateSecret(_secretId);
        output.close();

        logger->Info("Secret saved to %s", secretPath.c_str());
    }
    else
    {
        logger->Info("Failed to open secret file for writing");
    }
}

void WebServer::OnUpdate(unsigned int time)
{
    if (BaseURL.empty())
        return;

    if (time - _lastHandshakeTime < 30000)
        return;

    if (_handshakeRunning)
        return;

    _lastHandshakeTime = time;
    _handshakeRunning = true;

    std::ostringstream position;
    position << std::fixed << std::setprecision(2)
             << PlayerPosition.x << ";"
             << PlayerPosition.y << ";"
             << PlayerPosition.z;

    std::string query =
        "/handshake?secretId=" + _secretId +
        "&position=" + position.str();

    std::string finalUrl = BaseURL + query;

    std::thread([finalUrl]() {
        std::string handshakeResult = DownloadAndGetContent(finalUrl);

        Json::Value root;
        Json::Reader reader;

        if (!reader.parse(handshakeResult, root))
        {
            logger->Info(
                "Failed to parse handshake: %s",
                reader.getFormattedErrorMessages().c_str()
            );

            _handshakeRunning = false;
            return;
        }

        if (root.get("error", true).asBool())
        {
            _handshakeRunning = false;
            return;
        }

        std::map<std::string, WebPlayer> players;

        const Json::Value& playersJson = root["players"];

        for (const Json::Value& playerJson : playersJson)
        {
            WebPlayer player;

            player.id = playerJson.get("id", "").asString();
            player.name = playerJson.get("name", "").asString();

            const Json::Value& pos = playerJson["pos"];

            if (pos.isArray() && pos.size() >= 3)
            {
                player.x = pos[0].asFloat();
                player.y = pos[1].asFloat();
                player.z = pos[2].asFloat();
            }
            else
            {
                player.x = 0.0f;
                player.y = 0.0f;
                player.z = 0.0f;
            }

            player.admin = playerJson.get("admin", 0).asInt() != 0;

            if (!player.id.empty())
                players[player.id] = player;
        }

        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            _players = std::move(players);
        }

        _handshakeRunning = false;
    }).detach();
}

void WebServer::UpdatePlayerData()
{
    
}