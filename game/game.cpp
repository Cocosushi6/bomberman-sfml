#include "game.h"
#include "player.h"
#include "../utils/utils.h"
#include <iostream>
#include <memory>
#include <string>

using namespace std;

Game::Game() : m_terrain(std::make_shared<Terrain>()) {
}

Game::Game(std::shared_ptr<Terrain> terrain, std::map<int, entity_ptr_t> entities) : m_terrain(terrain), m_entities(entities) {
}

int Game::init()
{
	m_terrain->addObserver(shared_from_this());
	return 0;
}

void Game::addEntity(int id, entity_ptr_t entity) {
    m_entities.insert(std::pair<int, entity_ptr_t>(id, entity));
	notify(id, EVENT_PLAYER_JOIN, getTimestamp());
}

void Game::removeEntity(int id) {
   m_entities.erase(id); 
   notify(id, EVENT_PLAYER_PART, getTimestamp());
}

void Game::addBomb(int id, std::shared_ptr<Bomb> bomb) { 
	m_bombs.insert(std::pair<int, bomb_ptr_t>(id, bomb));
	bomb->addObserver(shared_from_this());
	bomb->setGame(shared_from_this());
	notify(id, EVENT_BOMB_ADD, getTimestamp());
// 	cout << "added bomb " << id << endl;
}

void Game::update(float delta) {
	//update bombs 
	std::vector<int> bombsToRemove;
    for(std::map<int, bomb_ptr_t>::iterator it = m_bombs.begin(); it != m_bombs.end(); it++) {		
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
	for(std::map<int, entity_ptr_t>::iterator it = m_entities.begin(); it != m_entities.end(); it++) {
		it->second->udpate();
	}
}

int Game::attribID() {
    m_lastID++;
    return m_lastID;
}

void Game::onNotify(int objectID, Subject *sub, Event ev, sf::Uint64 timestamp)
{
	if(ev == EVENT_BOMB_EXPLODED || ev == EVENT_BOMB_DIED) {
		notify(objectID, ev, timestamp);
	}
}


sf::Packet& operator<<(sf::Packet& packet, Game &game) {
	packet << *(game.getTerrain().lock());
	sf::Uint16 numBombs = game.getBombs().size();
	packet << numBombs;
	for(std::map<int, bomb_ptr_t>::iterator it = game.getBombs().begin(); it != game.getBombs().end(); it++) {
		packet << *it->second;
	}
	sf::Uint16 numPlayers = game.getEntities().size();
	packet << numPlayers;
	for(std::map<int, entity_ptr_t>::iterator it = game.getEntities().begin(); it != game.getEntities().end(); it++) {
		packet << *it->second;
	}
	return packet;
}

sf::Packet& operator>>(sf::Packet& packet, Game &game) {
	packet >> *(game.getTerrain().lock());
	sf::Uint16 numBombs;
	packet >> numBombs;
	for(int i = 0; i < numBombs; i++) {
		bomb_ptr_t b = std::make_shared<Bomb>(std::weak_ptr<Game>());
		packet >> *b;
		game.addBomb(b->getID(), b);
	}
	sf::Uint16 numPlayers;
	packet >> numPlayers;
	for(int i = 0; i < numPlayers; i++) {
		entity_ptr_t p = std::make_shared<Player>();
		packet >> *p;
		game.addEntity(p->getID(), p);
	}
	return packet;
}

std::string Game::toString()
{
	std::string result;
	for(std::map<int, bomb_ptr_t>::iterator it = m_bombs.begin(); it != m_bombs.end(); it++) {
		result += it->second->toString();
		result += "\n";
	}
	for(std::map<int, entity_ptr_t>::iterator it = m_entities.begin(); it != m_entities.end(); it++) {
		result += it->second->toString();
		result += "\n";
	}
	return result;
}

