#ifndef SRC_PLAYER_H
#define SRC_PLAYER_H

#include <memory>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

class Game; //warning : may cause problem, because one attrib is pointer to game (uncomplete type error, this type of things)
class PlayerManager;

const int SPRITE_SIZE = 64;
	
class Player {
	public:
		Player(float x, float y, Game* game, int id, PlayerManager* manager = nullptr);
		Player();
        ~Player();
		void move(float newX, float newY, float delta); 
		void udpate();
		float getX() { return m_x; }
		float getY() { return m_y; }
		float getSpeed() { return m_speed; }
		bool isMoving() { return m_moving; }
		std::string getDirection() { return m_direction; }
		void setX(float x) { m_x = x; }
		void setY(float y) { m_y = y; }
		void setNewX(float newX) { m_newX = newX; }
		void setNewY(float newY) { m_newY = newY; }
		void setSpeed(float speed) { m_speed = speed; }
		void setMoving(bool moving) { m_moving = moving; }
		void setDirection(std::string direction) { m_direction = direction; }
		void setDead(bool dead) { m_dead = dead; }
		void setLife(int hp) { m_hp = hp; }
		sf::FloatRect getBounds();
		sf::Vector2< float > getTileCoordinates();
		bool isDead() { return m_dead; }
		void giveDamage(int amount);
		int getHP();
		int getID() { return m_id; }
		void setID(int id) { m_id = id; }
		PlayerManager* getManager() { return m_pManager; }
		void setManager(PlayerManager* p) { m_pManager = p; }
		void setGame(Game* game) { m_game = game; }
		std::string toString();
		void setLastDelta(float last) { m_lastDelta = last; }
		float getNewX() { return m_newX; }
		float getNewY() { return m_newY; }
	private:
		int checkCollision(float newX, float newY);
		float m_x, m_y, m_newX, m_newY;
		float m_speed;
		bool m_moving;
		std::string m_direction;
		Game* m_game = nullptr;
		int m_hp;
		bool m_dead;
		int m_id;
		float m_lastDelta = 0.0f;
		PlayerManager* m_pManager = nullptr;
};

sf::Packet& operator<<(sf::Packet& packet, Player &player);
sf::Packet& operator>>(sf::Packet& packet, Player &player);

#endif /* include guard */ 
