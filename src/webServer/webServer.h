#pragma once

#include "aml-psdk/gta_base/Vector.h"
#include <string>
#include <map>
#include <sys/stat.h>

#include "webPlayer.h"

class WebServer {
public:
    static std::string BaseURL;
    static CVector PlayerPosition;

    static unsigned int _lastHandshakeTime;

    static std::map<std::string, WebPlayer> _players;

    static void Initialze();

    static void OnUpdate(unsigned int time);

private:
    static void UpdatePlayerData();
};