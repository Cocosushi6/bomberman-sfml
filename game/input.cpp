
#include "input.h"
#include "../utils/utils.h"

#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>

using namespace std;

Input::Input(std::weak_ptr<Player> player, std::weak_ptr<Game> game) : m_player(player), m_game(game) {}


void Input::parseInput(float delta, std::weak_ptr<Client> sendback) { //TODO find a better way to give sendback (but not as a function param)
	sf::Uint64 timestamp = getTimestamp();
	//TODO remove calls to m_player.lock() and lock it using auto ptr @ beginning of method
	if(!m_player.lock()->isDead()) {
		//bool used to set the player to moving if any movement key is pressed
		bool currentlyMoving = false;
		
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) && hasFocus) {
			m_keyStates[sf::Keyboard::S] = true;
			m_player.lock()->setDirection("down");
			m_player.lock()->move(0.0f, 1.0f, delta);
			currentlyMoving = true;
			
			//tell the player manager to save the state, as it's part of the listeners
			notify(m_player.lock()->getID(), EVENT_INPUT_PLAYER_FORWARD, timestamp);
			
			//send input state to server for it to execute too
			InputState state = stateForKey(sf::Keyboard::S, delta, timestamp);
			
			sendback.lock()->sendUDPMessage("INPUTSTATE", state); 
			cout << "moved"  << "(newY : " << m_player.lock()->getNewY() << ", oldY : " << m_player.lock()->getY() << ")" << endl;
		} else {
			if(m_keyStates[sf::Keyboard::S]) { //only on key release
				cout << timestamp << " key released, player pos : " << m_player.lock()->getX() << ", " << m_player.lock()->getY() << endl;
				InputState state = stateForKey(sf::Keyboard::S, delta, timestamp);
				sendback.lock()->sendUDPMessage("INPUTSTATE", state);
				
				m_keyStates[sf::Keyboard::S] = false;
			}
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && hasFocus) {
			m_keyStates[sf::Keyboard::Z] = true;
			m_player.lock()->setDirection("up");
			m_player.lock()->move(0.0f, -1.0f, delta);
			currentlyMoving = true;
			
			notify(m_player.lock()->getID(), EVENT_INPUT_PLAYER_BACKWARD, timestamp);
			
			InputState state = stateForKey(sf::Keyboard::Z, delta, timestamp);
			sendback.lock()->sendUDPMessage("INPUTSTATE", state);
			
		} else {
			if(m_keyStates[sf::Keyboard::Z]) {
				InputState state = stateForKey(sf::Keyboard::Z, delta, timestamp);
				sendback.lock()->sendUDPMessage("INPUTSTATE", state);
				
				m_keyStates[sf::Keyboard::Z] = false;
			}
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && hasFocus) {
			m_keyStates[sf::Keyboard::Q] = true;
			m_player.lock()->setDirection("left");
			m_player.lock()->move(-1.0f, 0.0f, delta);
			currentlyMoving = true;
			
			notify(m_player.lock()->getID(), EVENT_INPUT_PLAYER_LEFT, timestamp);
			
			InputState state = stateForKey(sf::Keyboard::Q, delta, timestamp);
			sendback.lock()->sendUDPMessage("INPUTSTATE", state);
		} else {
			if(m_keyStates[sf::Keyboard::Q]) {
				InputState state = stateForKey(sf::Keyboard::Q, delta, timestamp);
				sendback.lock()->sendUDPMessage("INPUTSTATE", state);
				
				m_keyStates[sf::Keyboard::Q] = false;
			}
		}
		
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) && hasFocus) {
			m_keyStates[sf::Keyboard::D] = true;
			m_player.lock()->setDirection("right");
			m_player.lock()->move(1.0f, 0.0f, delta);
			currentlyMoving = true;
			
			notify(m_player.lock()->getID(), EVENT_INPUT_PLAYER_RIGHT, timestamp);
			
			InputState state = stateForKey(sf::Keyboard::D, delta, timestamp);
			sendback.lock()->sendUDPMessage("INPUTSTATE", state);
		} else {
			if(m_keyStates[sf::Keyboard::D]) {
				InputState state = stateForKey(sf::Keyboard::D, delta, timestamp);
				sendback.lock()->sendUDPMessage("INPUTSTATE", state);
				
				m_keyStates[sf::Keyboard::D] = false;
			}
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && hasFocus) {
			if(m_elapsedSinceBombPosing >= 0.3f) {
				sendback.lock()->sendUDPMessage("ADDBOMB");
				notify(-1, EVENT_INPUT_BOMB_ADD, getTimestamp());
				m_elapsedSinceBombPosing = 0.0f;
			}
		}
		m_elapsedSinceBombPosing += delta;
		m_player.lock()->setMoving(currentlyMoving);
	}
	
	//just for debug
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
		m_game.lock()->getTerrain().lock()->printWorld();
	}
}
