#ifndef SRC_PLAYER_MNG_H
#define SRC_PLAYER_MNG_H

#include <memory>
#include "player_state.h"
#include "../game/game.h"
#include "../game/player.h"
#include "observer.h"

typedef Player* player_ptr_t;

class PlayerManager : public Observer {
	public:
		PlayerManager(Game* game, int playerID);
		player_ptr_t getPlayer() { return m_player; }
		int getPlayerID() { return m_playerID; }
		PlayerStateManager& getPStateMgr() { return m_pStateManager; }
		void changePlayer(int id);
		virtual void onNotify(int objectID, Subject *sub, Event ev, sf::Uint64 timestamp);
	private:
		player_ptr_t m_player;
		int m_playerID;
		game_ptr_t m_game;
		PlayerStateManager m_pStateManager;
};



#endif /* SRC_PLAYER_MNG_H */
