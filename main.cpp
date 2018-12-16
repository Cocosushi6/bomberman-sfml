#include <iostream>
#include <SFML/Graphics.hpp>
#include "render/renderer.h"
#include "game/game.h"
#include "game/input.h"
#include "net/client.h"
#include "utils/player_manag.h"
#include "net/server.h"

using namespace std;

int main_client();
int main_server();

int main() {
	cout << "Choose 1 for Client or 2 for Server : " << endl;
	int choice, returnCode;
	do {
		cin >> choice;
		if(choice == 1) {
			returnCode = main_client();
			cout << "return " << returnCode << endl;
		} else if(choice == 2) {
			returnCode = main_server();
			cout << "return " << returnCode << endl;
		} else {
			cout << "Please choose 1 or 2" << endl;
		}
	} while(choice != 1 && choice != 2);
	
	return returnCode;
}

int main_server() {
	std::shared_ptr<Game> game = std::make_shared<Game>();
	game->init();
	std::shared_ptr<Server> server = std::make_shared<Server>(1665, 1665, game);
	if(server->init() != 0) {
		return -1;
	}
	game->addObserver(server);
	
	bool running = true;
	sf::Clock clock;
	
	while(running) {
		sf::Time delta = clock.restart();
		game->update(delta.asSeconds());
		server->poll(delta.asSeconds());
	}
	return 0;
}

int main_client() {
    std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>(sf::VideoMode(800, 608), "test", sf::Style::Titlebar | sf::Style::Close);
    window->setFramerateLimit(60);
    
	std::shared_ptr<Client> client = std::make_shared<Client>("127.0.0.1", 1665);
	std::tuple<std::shared_ptr<Game>, int> result = client->connect();
    std::shared_ptr<Game> game = std::get<0>(result);
	int ID = std::get<1>(result);
	if(game == nullptr || ID == -1) {
		cout << "couldn't fetch data from server" << endl;
		return -1;
	}
	game->init();
	client->addObserver(game);
	
	cout << "game and id fetched successfully" << endl;
	
	std::shared_ptr<PlayerManager> playerManager = std::make_shared<PlayerManager>(game, ID);
	//TODO add playerManager->init() here
	if(auto player = game->getEntity(ID).lock()) {
		player->setManager(playerManager); //Some sort of "use" of the component pattern
		player->setGame(game);
	} else {
		return -1;
	}
	
	cout << "local player manager initialised" << endl;
	
    std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>(ID, game, window);
	if(renderer->init() != 0) {
		cout << "failed to create renderer" << endl;
		return -2;
	}
	game->addObserver(renderer);
    std::shared_ptr<Input> inputManager = std::make_shared<Input>(game->getEntity(ID).lock(), game);
	inputManager->addObserver(playerManager); //To add player state
	
	cout << "renderer and input manager initialised" << endl;
	
    sf::Clock clock;
	cout << "Init done ! " << endl;
	
	while(window->isOpen()) {
		//TODO take a look at delta pattern and implement it correctly in game
        sf::Time elapsed = clock.restart();
        
		sf::Event event;
		while(window->pollEvent(event)) {
			if(event.type == sf::Event::Closed) {
				window->close();
			} else if(event.type == sf::Event::GainedFocus) {
				hasFocus = true;
			} else if(event.type == sf::Event::LostFocus) {
				hasFocus = false;
			}
		}
		client->poll();
		
		window->clear(sf::Color::Black);
		inputManager->parseInput(elapsed.asSeconds(), client);
		game->update(elapsed.asSeconds());
        renderer->render(elapsed.asSeconds());
		
		window->display();
	}
	
	cout << "Ending game" << endl;

	return 0;
}


