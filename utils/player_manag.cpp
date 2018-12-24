#include "player_manag.h"
#include <string>
#include <memory>
#include "event.h"

PlayerManager::PlayerManager(game_ptr_t game, int playerID) : m_game(game), m_playerID(playerID), m_pStateManager()
{
	//TODO return condition does nothing right in constructor, need to replace by init method
	if(m_game != nullptr) {
		m_player = m_game->getEntity(playerID);
	} else {
		return;
	}
}


void PlayerManager::changePlayer(int id)
{
	m_playerID = id;
	m_player = m_game->getEntity(id);
}

void PlayerManager::onNotify(int objectID, Subject *sub, Event ev, sf::Uint64 timestamp) {
	if(ev == EVENT_INPUT_PLAYER_FORWARD || ev == EVENT_INPUT_PLAYER_BACKWARD || ev == EVENT_INPUT_PLAYER_LEFT || ev == EVENT_INPUT_PLAYER_RIGHT) {
		m_pStateManager.save(timestamp, *m_player);
	}
}

