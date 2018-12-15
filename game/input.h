#ifndef SRC_INPUT_H
#define SRC_INPUT_H

#include <memory>
#include <iostream>

#include "game.h"
#include "../net/client.h"
#include "../utils/subject.h"

class Input : public Subject {
	public:
		Input(std::weak_ptr<Player> player, std::weak_ptr<Game> game);
		void parseInput(float delta, std::weak_ptr<Client> sendback);
	private:
        std::weak_ptr<Player> m_player;
		std::weak_ptr<Game> m_game;
		float m_elapsedSinceBombPosing = 0.0f; // This has nothing to do here
		std::map<sf::Keyboard::Key, bool> m_keyStates;
};



#endif

