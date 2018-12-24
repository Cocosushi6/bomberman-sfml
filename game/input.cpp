
#include "input.h"
#include "../utils/utils.h"

#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>

using namespace std;

Input::Input(Player* player, Game* game) : m_player(player), m_game(game) {}


void Input::parseInput(float delta, Client* sendback) { //TODO find a better way to give sendback (but not as a function param)
	sf::Uint64 timestamp = getTimestamp();
	//TODO remove calls to m_player and lock it using auto ptr @ beginning of method
	if(!m_player->isDead()) {
		//bool used to set the player to moving if any movement key is pressed
		bool currentlyMoving = false;
		
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) && hasFocus) {
			m_keyStates[sf::Keyboard::S] = true;
			m_player->setDirection("down");
			m_player->move(0.0f, 1.0f, delta);
			currentlyMoving = true;
			
			//tell the player manager to save the state, as it's part of the listeners
			notify(m_player->getID(), EVENT_INPUT_PLAYER_FORWARD, timestamp);
			
			//send input state to server for it to execute too
			InputState state = stateForKey(sf::Keyboard::S, delta, timestamp);
			
			sendback->sendUDPMessage("INPUTSTATE", state); 
		} else {
			if(m_keyStates[sf::Keyboard::S]) { //only on key release
				cout << timestamp << " key released, player pos : " << m_player->getX() << ", " << m_player->getY() << endl;
				InputState state = stateForKey(sf::Keyboard::S, delta, timestamp);
				sendback->sendUDPMessage("INPUTSTATE", state);
				
				m_keyStates[sf::Keyboard::S] = false;
			}
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && hasFocus) {
			m_keyStates[sf::Keyboard::Z] = true;
			m_player->setDirection("up");
			m_player->move(0.0f, -1.0f, delta);
			currentlyMoving = true;
			
			notify(m_player->getID(), EVENT_INPUT_PLAYER_BACKWARD, timestamp);
			
			InputState state = stateForKey(sf::Keyboard::Z, delta, timestamp);
			sendback->sendUDPMessage("INPUTSTATE", state);
			
		} else {
			if(m_keyStates[sf::Keyboard::Z]) {
				InputState state = stateForKey(sf::Keyboard::Z, delta, timestamp);
				sendback->sendUDPMessage("INPUTSTATE", state);
				
				m_keyStates[sf::Keyboard::Z] = false;
			}
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && hasFocus) {
			m_keyStates[sf::Keyboard::Q] = true;
			m_player->setDirection("left");
			m_player->move(-1.0f, 0.0f, delta);
			currentlyMoving = true;
			
			notify(m_player->getID(), EVENT_INPUT_PLAYER_LEFT, timestamp);
			
			InputState state = stateForKey(sf::Keyboard::Q, delta, timestamp);
			sendback->sendUDPMessage("INPUTSTATE", state);
		} else {
			if(m_keyStates[sf::Keyboard::Q]) {
				InputState state = stateForKey(sf::Keyboard::Q, delta, timestamp);
				sendback->sendUDPMessage("INPUTSTATE", state);
				
				m_keyStates[sf::Keyboard::Q] = false;
			}
		}
		
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) && hasFocus) {
			m_keyStates[sf::Keyboard::D] = true;
			m_player->setDirection("right");
			m_player->move(1.0f, 0.0f, delta);
			currentlyMoving = true;
			
			notify(m_player->getID(), EVENT_INPUT_PLAYER_RIGHT, timestamp);
			
			InputState state = stateForKey(sf::Keyboard::D, delta, timestamp);
			sendback->sendUDPMessage("INPUTSTATE", state);
		} else {
			if(m_keyStates[sf::Keyboard::D]) {
				InputState state = stateForKey(sf::Keyboard::D, delta, timestamp);
				sendback->sendUDPMessage("INPUTSTATE", state);
				
				m_keyStates[sf::Keyboard::D] = false;
			}
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && hasFocus) {
			if(m_elapsedSinceBombPosing >= 0.3f) {
				sendback->sendTCPMessage("ADDBOMB");
				notify(-1, EVENT_INPUT_BOMB_ADD, getTimestamp());
				m_elapsedSinceBombPosing = 0.0f;
			}
		}
		m_elapsedSinceBombPosing += delta;
		m_player->setMoving(currentlyMoving);
	}
	
	//just for debug
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::C) && hasFocus) {
		m_game->getTerrain()->printWorld();
	}
}
