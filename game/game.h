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
#include "entity.h"
#include "bomb.h"
#include "../utils/subject.h" 
#include "../utils/observer.h"

typedef std::unique_ptr<Entity> entity_ptr_t;
typedef std::unique_ptr<Bomb> bomb_ptr_t;

class Game : public Subject, public Observer {
	public:
		Game();
		Game(std::unique_ptr<Terrain> terrain, std::map<int, entity_ptr_t> entities);
		Game(Game && rhs); //move ctor

        int attribID();
		void update(float delta);

        Terrain* getTerrain() { return m_terrain.get(); }

		void addPlayer(int id, entity_ptr_t player);
		void addEntity(int id, entity_ptr_t entity);
		void removeEntity(int id);
        Entity* getEntity(int id);
		std::map<int, Entity*> getEntities();

        void addBomb(int id, bomb_ptr_t bomb);
		void removeBomb(int id) { m_bombs.erase(id);  std::cout << "removed bomb "<< id << std::endl; };
		Bomb* getBomb(int id);
		std::map<int, Bomb*> getBombs();

		virtual void onNotify(int objectID, Subject *sub, ::Event ev, sf::Uint64 timestamp);

		std::string toString();
	private:
		std::map<int, entity_ptr_t> m_entities;
		std::map<int, bomb_ptr_t> m_bombs;
        std::unique_ptr<Terrain> m_terrain;
        int m_lastID = 0;
		bool m_initDone = false;
};

typedef Game* game_ptr_t;

sf::Packet& operator<<(sf::Packet& packet, Game &game);
sf::Packet& operator>>(sf::Packet& packet, Game &game);

#endif /* GAME_GAME_H_ */
