#ifndef CLNT_LAUNCHER_H
#define CLNT_LAUNCHER_H

#include <iostream>
#include <string>
#include <memory>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "../net/client.h"
#include "game.h"
#include "../render/renderer.h"
#include "input.h"
#include "../utils/player_manag.h"

class ClientGame {
    public:
        ClientGame();
        int run(sf::IpAddress address, unsigned int tcpPort, unsigned int udpPort, unsigned int localUdpPort, std::string nickname);

    protected:
        int connect();
        int initGraphics();
        int registerObservers();

    private:
        sf::RenderWindow *m_window = nullptr;
        std::unique_ptr<Client> m_client = nullptr;
        std::unique_ptr<Game> m_game = nullptr;
        std::unique_ptr<PlayerManager> m_playerManager = nullptr;
        std::unique_ptr<Renderer> m_renderer = nullptr;
        std::unique_ptr<Input> m_inputManager = nullptr;
        Player *m_player = nullptr;
        sf::Clock m_clock;
        unsigned int m_id = 0;
        unsigned int m_tcpPort = 0;
        unsigned int m_udpPort = 0;
        unsigned int m_localUdpPort = 0;
        sf::IpAddress m_address;
        std::string m_nickname;
};

#endif