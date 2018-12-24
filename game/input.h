#ifndef SRC_INPUT_H
#define SRC_INPUT_H

#include <memory>
#include <iostream>

#include "game.h"
#include "../net/client.h"
#include "../utils/subject.h"

class Input : public Subject {
	public:
		Input(Player* layer, Game* game);
		void parseInput(float delta, Client* sendback);
	private:
		Player* m_player;
		Game* m_game;
		float m_elapsedSinceBombPosing = 0.0f; // This has nothing to do here
		std::map<sf::Keyboard::Key, bool> m_keyStates;
};



#endif

