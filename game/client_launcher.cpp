#include "client_launcher.h"
#include <memory>
#include "../utils/utils.h"

using namespace std;

ClientGame::ClientGame() {

}

int ClientGame::run(sf::IpAddress address, unsigned int tcpPort, unsigned int udpPort, unsigned int localUdpPort, std::string nickname) {
    m_address = address;
    m_tcpPort = tcpPort;
    m_udpPort = udpPort;
    m_localUdpPort = localUdpPort;
    m_nickname = nickname;

    int returnCode = 0;

    returnCode = connect();
    if(returnCode != 0) return returnCode;
    cout << "Connected to server." << endl;

    returnCode = initGraphics();
    if(returnCode != 0) return returnCode;
    cout << "Graphics and Input manager initialised" << endl;

    returnCode = registerObservers();
    if(returnCode != 0) return returnCode;
    cout << "Observers registered. Init Done !" << endl;

    	
	while(m_renderer->getWindow()->isOpen()) {
		//TODO take a look at delta pattern and implement it correctly in game
		sf::Time elapsed = m_clock.restart();
        
		sf::Event event;
		while(m_renderer->getWindow()->pollEvent(event)) {
			if(event.type == sf::Event::Closed) {
				m_renderer->getWindow()->close();
			} else if(event.type == sf::Event::GainedFocus) {
				hasFocus = true;
			} else if(event.type == sf::Event::LostFocus) {
				hasFocus = false;
			}
		}
		m_client->poll();
		
		m_renderer->getWindow()->clear(sf::Color::Black);
		m_inputManager->parseInput(elapsed.asSeconds(), m_client.get());
		m_game->update(elapsed.asSeconds());
        m_renderer->render(elapsed.asSeconds());
		
		m_renderer->getWindow()->display();
	}
	
	cout << "Ending game." << endl;

    return 0;
}

int ClientGame::connect() {
    m_client = make_unique<Client>(m_address, m_tcpPort, m_udpPort, m_localUdpPort);
    auto result = m_client->connect();

    m_game = move(get<0>(result));
    m_id = get<1>(result);

    if(m_game == nullptr || m_id == -1) return -1;

    m_player = dynamic_cast<Player*>(m_game->getEntity(m_id));
    m_playerManager = make_unique<PlayerManager>(m_game.get(), m_id);

    return 0;
}

int ClientGame::registerObservers() {
    m_client->addObserver(m_game.get());
    m_player->setManager(m_playerManager.get());
    m_player->setGame(m_game.get());
    m_game->addObserver(m_renderer.get());
    m_inputManager->addObserver(m_playerManager.get());
    
    return 0;
}

int ClientGame::initGraphics() {
    unique_ptr<sf::RenderWindow> window = make_unique<sf::RenderWindow>(
        sf::VideoMode(800, 608),
        "Bomberman",
        sf::Style::Titlebar | sf::Style::Close
    );
    window->setFramerateLimit(60);
    m_window = window.get();

    m_renderer = make_unique<Renderer>(m_id, m_game.get(), move(window));
    m_inputManager = make_unique<Input>(m_player, m_game.get());

    if(m_renderer->init() != 0) return -2;

    return 0;
}
