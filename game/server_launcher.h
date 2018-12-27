#ifndef SRV_LAUNCHER_H
#define SRV_LAUNCHER_H

#include <iostream>
#include <memory>
#include "game.h"
#include "../net/server.h"

class ServerGame {
    public:
        ServerGame();
        int run(unsigned int tcpPort = 1665, unsigned int udpPort = 1665);
        void stop();
    private:
        bool m_running = false;
        std::unique_ptr<Game> m_game = nullptr;
        std::unique_ptr<Server> m_server = nullptr;
};

#endif 