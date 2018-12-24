#include "bomb.h"
#include "game.h"
#include "map.h"
#include "../utils/utils.h"
#include <iostream>

using namespace std;

Bomb::Bomb(int x, int y, float duration, Game* game, int id) : m_x(x), m_y(y), m_duration(duration), m_game(game), m_id(id), m_elapsedTime(0.0f)
{
}

Bomb::Bomb(sf::Vector2i coordinates, float duration, Game* game, int id) : m_x(coordinates.x), m_y(coordinates.y), m_duration(duration), m_game(game), m_id(id), m_elapsedTime(0.0f)
{
}

Bomb::Bomb(Game* game) : m_x(-200), m_y(-200), m_id(-1), m_duration(BOMB_DURATION), m_game(game), m_elapsedTime(0.0f)
{
}

Bomb::~Bomb()
{
}

void Bomb::explode()
{
	if(!m_exploded) {
		sf::FloatRect horizontal = sf::FloatRect(m_x-32, m_y, 96, 32);
		sf::FloatRect vertical = sf::FloatRect(m_x, m_y-32, 32, 96);
			map<int, Player*> gameEntities = m_game->getEntities();
			for(auto it = gameEntities.begin(); it != gameEntities.end(); it++) {
				if(it->second->getBounds().intersects(horizontal) || it->second->getBounds().intersects(vertical)) {
					it->second->giveDamage(50);
				} 
			}
			
			sf::Vector2i tileCoords = sf::Vector2i(m_x / TILE_SIZE, m_y / TILE_SIZE);
			for(vector<Tile*> line : m_game->getTerrain()->getTiles()) {
				for(auto t : line) {
					if(t->isBreakable()) {
						if(t->getWorldCoordinates().x >= tileCoords.x-1 && t->getWorldCoordinates().x <= tileCoords.x +1) {
							if(t->getWorldCoordinates().y >= tileCoords.y - 1 && t->getWorldCoordinates().y <= tileCoords.y + 1) {
								m_game->getTerrain()->breakTileAt(t->getWorldCoordinates().x, t->getWorldCoordinates().y, "stone");
							}
						}
					}
				}
			}
		m_exploded = true;
	}
}

void Bomb::update(float delta)
{
	if(!m_dead) {
		m_elapsedTime += delta;
		if(m_elapsedTime >= m_duration && !m_exploded) {
			explode();
			notify(m_id, EVENT_BOMB_EXPLODED, getTimestamp());
		}
		if(m_elapsedTime >= m_duration + m_periodBeforeDead && !m_dead) {
			die();
			notify(m_id, EVENT_BOMB_DIED, getTimestamp());
		}
	}
}

sf::IntRect Bomb::getBounds()
{
	return sf::IntRect(m_x, m_y, 32, 32);
}

sf::Packet& operator<<(sf::Packet& packet, Bomb &bomb) {
	return packet << bomb.getID() << bomb.getX() << bomb.getY() << bomb.getDuration() << bomb.getElapsedTime() << bomb.hasExploded() << bomb.isDead() << bomb.getPeriodBeforeDead();
}

sf::Packet& operator>>(sf::Packet& packet, Bomb &bomb) {
	int x, y, id;
	float duration, elapsedTime, periodBforeDead;
	bool exploded, dead;
	packet >> id >> x >> y >> duration >> elapsedTime >> exploded >> dead >> periodBforeDead;
	bomb.setX(x);
	bomb.setY(y);
	bomb.setID(id);
	bomb.setDuration(duration);
	bomb.setElapsedTime(elapsedTime);
	bomb.setExploded(exploded);
	bomb.setDead(dead);
	bomb.setPeriodBeforeDead(periodBforeDead);
	return packet;
}

string Bomb::toString()
{
	string result;
	result += to_string(m_id) + " ";
	result += to_string(m_x) + " ";
	result += to_string(m_y) + " ";
	result += to_string(m_elapsedTime) + " ";
	result += ((m_exploded) ? "exploded" : "not_exploded") + (string)" ";
	result += ((m_dead) ? "dead" : "not_dead") + (string)" ";
	return result;
}

