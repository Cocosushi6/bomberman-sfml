#include "server_launcher.h"
#include <memory>
#include <iostream>

using namespace std;

ServerGame::ServerGame() {
}

int ServerGame::run(unsigned int tcpPort, unsigned int udpPort) {
    //launch empty game
    m_game = make_unique<Game>();

    //launch server, with game as parameter
    m_server = make_unique<Server>(tcpPort, udpPort, m_game.get());
    int launchServerSuccess = m_server->init();
    if(launchServerSuccess != 0) return launchServerSuccess;
    m_game->addObserver(m_server.get());

    m_running = true;
    sf::Clock clock;

    while(m_running) {
        sf::Time delta = clock.restart();
        m_game->update(delta.asSeconds());
        m_server->poll(delta.asSeconds());
    }

    return 0;
}

void ServerGame::stop() {
    m_running = false; 
}