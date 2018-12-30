#ifndef SRC_PLAYER_H
#define SRC_PLAYER_H

#include "entity.h"
#include "player_characteristics.h"

#include <memory>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

class Game; 
class PlayerManager;

const int SPRITE_SIZE = 64;
	
class Player : public Entity {
	public:
		Player(float x, float y, float width, float height, Game* game, int id, PlayerManager* manager = nullptr);
		Player(sf::Vector2f position, sf::Vector2f size, Game* game, int id, PlayerManager *manager = nullptr);
		Player();
		Player(const Player& other); 
        virtual ~Player() = default;

		virtual void move(float newX, float newY, float delta); 
		virtual void update();
	
		PlayerManager* getManager() { return m_pManager; }
		void setManager(PlayerManager* p) { m_pManager = p; }
		std::string toString();
		void setLastDelta(float last) { m_lastDelta = last; }
		PlayerCharacteristics* getCharacteristics() { return m_data.get(); }

	protected:
		virtual int checkCollision(float newX, float newY);
		std::unique_ptr<PlayerCharacteristics> m_data;
		PlayerManager* m_pManager = nullptr;
};

sf::Packet& operator<<(sf::Packet& packet, Direction &dir);
sf::Packet& operator>>(sf::Packet& packet, Direction &dir);

sf::Packet& operator<<(sf::Packet& packet, Player &player);
sf::Packet& operator>>(sf::Packet& packet, Player &player);

#endif /* include guard */ 
