#include "player_state.h"
#include "../game/player.h"
#include <string>
#include <map>

using namespace std;

PlayerStateManager::PlayerStateManager() {}

void PlayerStateManager::save(sf::Uint64 timestamp, Player p) {
	m_states.insert(std::pair<sf::Uint64, Player>(timestamp, p));
}

Player PlayerStateManager::get(sf::Uint64 timestamp)
{
	return m_states.at(timestamp);
}


