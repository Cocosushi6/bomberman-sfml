#include "game.h"
#include "player.h"
#include "../utils/utils.h"
#include <iostream>
#include <memory>
#include <string>

using namespace std;

Game::Game() : m_terrain(make_unique<Terrain>()) {
	m_terrain->addObserver(this);
}

Game::Game(unique_ptr<Terrain> terrain, map<int, entity_ptr_t> entities) : m_terrain(move(terrain)), m_entities(move(entities)) {
	m_terrain->addObserver(this);
}

Game::Game(Game && rhs) : m_lastID(rhs.m_lastID), m_initDone(rhs.m_initDone), m_terrain(move(rhs.m_terrain)), m_bombs(move(rhs.m_bombs)), m_entities(move(rhs.m_entities)) {
	m_terrain->addObserver(this);
}

void Game::addEntity(int id, entity_ptr_t entity) {
	m_entities[id] = move(entity);
	notify(id, EVENT_PLAYER_JOIN, getTimestamp());
}

void Game::removeEntity(int id) {
   m_entities.erase(id); 
   notify(id, EVENT_PLAYER_PART, getTimestamp());
}

void Game::addBomb(int id, unique_ptr<Bomb> bomb) { 
	bomb->addObserver(this);
	bomb->setGame(this);
	m_bombs[id] = move(bomb);
	notify(id, EVENT_BOMB_ADD, getTimestamp());
// 	cout << "added bomb " << id << endl;
}

void Game::update(float delta) {
	//update bombs 
	vector<int> bombsToRemove;
    for(auto it = m_bombs.begin(); it != m_bombs.end(); it++) {		
		it->second->update(delta);
		if(it->second->isDead()) {
			cout << "bomb is dead, removing" << endl;
			bombsToRemove.push_back(it->first);
		}
	}
	for(int i : bombsToRemove) {
		removeBomb(i);
	}
	
	//update entities
	for(auto it = m_entities.begin(); it != m_entities.end(); it++) {
		it->second->update();
	}
}

int Game::attribID() {
    m_lastID++;
    return m_lastID;
}

map<int, Entity*> Game::getEntities() {
	map<int, Entity*> returnMap;
	for(auto it = m_entities.begin(); it != m_entities.end(); it++) {
		returnMap[it->first] = it->second.get();
	}
	return returnMap;
}

map<int, Bomb*> Game::getBombs() {
	map<int, Bomb*> returnMap;
	for(auto it = m_bombs.begin(); it != m_bombs.end(); it++) {
		returnMap[it->first] = it->second.get();
	}
	return returnMap;
}


void Game::onNotify(int objectID, Subject *sub, Event ev, sf::Uint64 timestamp)
{
	if(ev == EVENT_BOMB_EXPLODED || ev == EVENT_BOMB_DIED) {
		notify(objectID, ev, timestamp);
	}
}


sf::Packet& operator<<(sf::Packet& packet, Game &game) {
	packet << *game.getTerrain();
	sf::Uint16 numBombs = game.getBombs().size();
	packet << numBombs;
	auto gameBombs = game.getBombs();
	for(auto it = gameBombs.begin(); it != gameBombs.end(); it++) {
		packet << *it->second;
	}
	sf::Uint16 numPlayers = game.getEntities().size();
	packet << numPlayers;
	for(auto it = game.getEntities().begin(); it != game.getEntities().end(); it++) {
		packet << *dynamic_cast<Player*>(it->second);
	}
	return packet;
}

sf::Packet& operator>>(sf::Packet& packet, Game &game) {
	packet >> *(game.getTerrain());
	sf::Uint16 numBombs;
	packet >> numBombs;
	for(int i = 0; i < numBombs; i++) {
		bomb_ptr_t b = make_unique<Bomb>(&game);
		packet >> *b;
		int id = b->getID();
		game.addBomb(id, move(b));
	}
	sf::Uint16 numPlayers;
	packet >> numPlayers;
	for(int i = 0; i < numPlayers; i++) {
		unique_ptr<Player> p = make_unique<Player>();
		packet >> *p;

		int id = p->getID();
		game.addEntity(id, move(p));
	}
	return packet;
}

string Game::toString()
{
	string result;
	for(map<int, bomb_ptr_t>::iterator it = m_bombs.begin(); it != m_bombs.end(); it++) {
		result += it->second->toString();
		result += "\n";
	}
	for(map<int, entity_ptr_t>::iterator it = m_entities.begin(); it != m_entities.end(); it++) {
		result += it->second->toString();
		result += "\n";
	}
	return result;
}

Bomb* Game::getBomb(int id) {
	try {
		return m_bombs.at(id).get();
	} catch(out_of_range e) {
		cout << "No such bomb with id " << id << endl;
		return nullptr;
	}
}

Entity* Game::getEntity(int id) {
	try {
		return m_entities.at(id).get();
	} catch(out_of_range e) {
		cout << "No such player with id " << id << endl;
		return nullptr;
	}
}