#ifndef SRC_BOMB_H
#define SRC_BOMB_H

#include <memory>
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "../utils/utils.h"
#include "../utils/subject.h"

class Game;

const float BOMB_DURATION = 2.0f;

class Bomb : public Subject {
	public:
		Bomb(int x, int y, float duration, std::weak_ptr<Game> game, int id);
		Bomb(sf::Vector2i coordinates, float duration, std::weak_ptr<Game> game, int id);
		Bomb(std::weak_ptr<Game> game);
		~Bomb();
		void update(float delta);
		void explode();
		sf::IntRect getBounds();
		sf::FloatRect getFBounds() { return sf::FloatRect(m_x, m_y, 32, 32); }
		int getX() { return m_x; }
		int getY() { return m_y; }
		bool hasExploded() { return m_exploded; }
		float getDuration() { return m_duration; }
		float getElapsedTime() { return m_elapsedTime; }
		float getPeriodBeforeDead() { return m_periodBeforeDead; }
		sf::Vector2i getTileCoordinates() { return toTileCoordinates(m_x, m_y); }
		bool isDead() { return m_dead; }
		void die() { m_dead = true; }
		void setX(int x) { m_x = x; }
		void setY(int y) { m_y = y; }
		void setExploded(bool exploded) { m_exploded = exploded; }
		void setDead(bool dead) { m_dead = dead; }
		void setDuration(float duration) { m_duration = duration; }
		void setElapsedTime(float elTime) { m_elapsedTime = elTime; }
		void setPeriodBeforeDead(float pbdead) { m_periodBeforeDead = pbdead; }
		int getID() { return m_id; }
		void setID(int id) { m_id = id; }
		std::string toString();
		void setGame(std::weak_ptr<Game> game) { m_game = game; }
	private:
		int m_x, m_y;
		int m_id = -1;
		float m_duration;
		float m_elapsedTime = 0.0f;
		std::weak_ptr<Game> m_game;
		bool m_exploded = false;
		bool m_dead = false;
		float m_periodBeforeDead = 0.3f;
};

sf::Packet& operator<<(sf::Packet& packet, Bomb &bomb);
sf::Packet& operator>>(sf::Packet& packet, Bomb &bomb);


#endif /* SRC_BOMB_H */
