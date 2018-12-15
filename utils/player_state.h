#ifndef  player_state_INC
#define  player_state_INC

#include <map>
#include <string>
#include <SFML/System.hpp>
#include "../game/player.h"

class PlayerStateManager {
	public:
		PlayerStateManager();
		void save(sf::Uint64 timestamp, Player p);
		Player get(sf::Uint64 timestamp);
		std::map<sf::Uint64, Player> getStates() { return m_states; }
	private:
		std::map<sf::Uint64, Player> m_states;
};

#endif   /* ----- #ifndef player_state_INC  ----- */

