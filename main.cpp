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

int main2() {
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
	unique_ptr<Game> game = make_unique<Game>();
	unique_ptr<Server> server = make_unique<Server>(1665, 1665, game.get());
	if(server->init() != 0) {
		return -1;
	}
	game->addObserver(server.get());
	
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
    unique_ptr<sf::RenderWindow> window = make_unique<sf::RenderWindow>(sf::VideoMode(800, 608), "test", sf::Style::Titlebar | sf::Style::Close);
    window->setFramerateLimit(60);
    
	unique_ptr<Client> client = make_unique<Client>("127.0.0.1", 1665);
	tuple<unique_ptr<Game>, int> result = client->connect();
	unique_ptr<Game> game = move(get<0>(result));
	int ID = get<1>(result);
	if(game == nullptr || ID == -1) {
		cout << "couldn't fetch data from server" << endl;
		return -1;
	}
	client->addObserver(game.get());
	
	cout << "game and id fetched successfully" << endl;
	
	unique_ptr<PlayerManager> playerManager = make_unique<PlayerManager>(game.get(), ID);
	//TODO add playerManager->init() here
	auto player = game->getEntity(ID);
	if(player != nullptr) {
		player->setManager(playerManager.get()); //Some sort of "use" of the component pattern
		player->setGame(game.get());
	} else {
		return -1;
	}
	
	cout << "local player manager initialised" << endl;
	
	unique_ptr<Renderer> renderer = make_unique<Renderer>(ID, game.get(), move(window));
	if(renderer->init() != 0) {
		cout << "failed to create renderer" << endl;
		return -2;
	}
	game->addObserver(renderer.get());
	unique_ptr<Input> inputManager = make_unique<Input>(game->getEntity(ID), game.get());
	inputManager->addObserver(playerManager.get()); //To add player state
	
	cout << "renderer and input manager initialised" << endl;
	
	sf::Clock clock;
	cout << "Init done ! " << endl;
	
	while(renderer->getWindow()->isOpen()) {
		//TODO take a look at delta pattern and implement it correctly in game
		sf::Time elapsed = clock.restart();
        
		sf::Event event;
		while(renderer->getWindow()->pollEvent(event)) {
			if(event.type == sf::Event::Closed) {
				renderer->getWindow()->close();
			} else if(event.type == sf::Event::GainedFocus) {
				hasFocus = true;
			} else if(event.type == sf::Event::LostFocus) {
				hasFocus = false;
			}
		}
		client->poll();
		
		renderer->getWindow()->clear(sf::Color::Black);
		inputManager->parseInput(elapsed.asSeconds(), client.get());
		game->update(elapsed.asSeconds());
        renderer->render(elapsed.asSeconds());
		
		renderer->getWindow()->display();
	}
	
	cout << "Ending game" << endl;

	return 0;
}


