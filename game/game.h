/*
 * game.h
 *
 *  Created on: Jun 30, 2017
 *      Author: sacha
 */

#ifndef GAME_GAME_H_
#define GAME_GAME_H_

#include <memory>
#include <map>
#include <string>
#include "map.h"
#include "player.h"
#include "bomb.h"
#include "../utils/subject.h" 
#include "../utils/observer.h"

typedef std::shared_ptr<Player> entity_ptr_t;
typedef std::shared_ptr<Bomb> bomb_ptr_t;

class Game : public std::enable_shared_from_this<Game>, public Subject, public Observer {
	public:
		Game();
		Game(std::shared_ptr<Terrain> terrain, std::map<int, entity_ptr_t> entities);
		int init();
        int attribID();
		void addEntity(int id, entity_ptr_t entity);
		void removeEntity(int id);
		void update(float delta);
		std::map<int, entity_ptr_t> getEntities() {	return m_entities;	}
        std::weak_ptr<Player> getEntity(int id) { return m_entities[id]; }
        std::weak_ptr<Terrain> getTerrain() { return m_terrain; }
        void addBomb(int id, std::shared_ptr<Bomb> bomb);
		void removeBomb(int id) { m_bombs.erase(id);  std::cout << "removed bomb "<< id << std::endl; };
		std::map<int, bomb_ptr_t>& getBombs() { return m_bombs; }
		std::weak_ptr<Bomb> getBomb(int id) { return m_bombs.at(id); }
		virtual void onNotify(int objectID, Subject *sub, Event ev, sf::Uint64 timestamp);
		std::string toString();
	private:
		std::map<int, entity_ptr_t> m_entities;
		std::map<int, bomb_ptr_t> m_bombs;
        std::shared_ptr<Terrain> m_terrain;
        int m_lastID = 0;
		bool m_initDone = false;
};

typedef std::weak_ptr<Game> game_ptr_t;

sf::Packet& operator<<(sf::Packet& packet, Game &game);
sf::Packet& operator>>(sf::Packet& packet, Game &game);

#endif /* GAME_GAME_H_ */
