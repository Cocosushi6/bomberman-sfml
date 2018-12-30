#include "player.h"
#include "game.h"
#include "../utils/utils.h"
#include "../utils/player_manag.h"
#include <SFML/Graphics.hpp>
#include "player_characteristics.h"

using namespace std;

constexpr unsigned int str2int(const char* str, int h = 0)
{
	return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

Player::Player(float x, float y, float width, float height, Game* game, int id, PlayerManager* manager) : Player(sf::Vector2f(x, y), sf::Vector2f(width, height), game, id, manager) 
{}

Player::Player(sf::Vector2f position, sf::Vector2f size, Game *game, int id, PlayerManager* manager) : Entity(position, size, game, id), m_pManager(manager) {
	m_data = make_unique<PlayerCharacteristics>(100, false);
}

Player::Player() : Entity(), m_pManager(nullptr) {
	m_data = make_unique<PlayerCharacteristics>(100, false);
}

Player::Player(const Player& other) : Entity(other), m_pManager(other.m_pManager)
{
	m_data = make_unique<PlayerCharacteristics>(*other.m_data);
}

int Player::checkCollision(float newX, float newY)
{
	if (m_game != nullptr)
	{
		//TODO check collision with other players too
		sf::FloatRect nextBounds(this->getBounds());
		switch (m_direction)
		{
		case LEFT:
			nextBounds.left += newX;
			break;
		case RIGHT:
			nextBounds.width += newX;
			break;
		case UP:
			nextBounds.top += newY;
			break;
		case DOWN:
			nextBounds.height += newY;
			break;
		}

		for (auto line : m_game->getTerrain()->getTiles())
		{
			for (auto &&t : line)
			{
				if (t->isSolid())
				{
					if (nextBounds.intersects(t->getBounds()))
					{
						return 1;
					}
				}
			}
		}
		auto gameBombs = m_game->getBombs();
		for (auto it = gameBombs.begin(); it != gameBombs.end(); it++)
		{
			if (!this->getBounds().intersects(it->second->getFBounds()))
			{ //this condition avoids making collision when the player has just put the bomb under its feet
				if (nextBounds.intersects(it->second->getFBounds()))
				{
					return 1;
				}
			}
		}

		for (auto &p : m_game->getEntities())
		{
			if (p.first != m_id)
			{
				if (nextBounds.intersects(p.second->getBounds()))
				{
					return 2;
				}
			}
		}
	}
	else
	{
		return 3;
	}

	return 0;

}

void Player::update() {
	float velocity = m_lastDelta * m_speed;
	if(m_position.x > m_newPosition.x) {
		if(m_position.x - m_newPosition.x > velocity) {
			m_position.x -= velocity;
		} else {
			m_position.x -= m_position.x - m_newPosition.x;
		}
	} else {
		if(m_newPosition.x - m_position.x > velocity) {
			m_position.x += velocity;
		} else {
			m_position.x += m_newPosition.x - m_position.x;
		}
	}
	
	if(m_position.y > m_newPosition.y) {
		if(m_position.y - m_newPosition.y > velocity) {
			m_position.y -= velocity;
		} else {
			m_position.y -= m_position.y - m_newPosition.y;
		}
	} else {
		if(m_newPosition.y - m_position.y > velocity) {
			m_position.y += velocity;
		} else {
			m_position.y += m_newPosition.y - m_position.y;
		}
	}
}


//move parameters are added to current position;
void Player::move(float newX, float newY, float delta) {
	float velocity = delta * m_speed; 
	newX *= velocity;
	newY *= velocity;
	
	if(checkCollision(newX, newY) != 0) {
		return;
	}
	
	this->m_newPosition.x = newX + m_position.x;
	this->m_newPosition.y = newY + m_position.y;
	m_lastDelta = delta;
}

sf::Packet & operator<<(sf::Packet& packet, Direction &dir) {
	return packet << (int)dir;
}

sf::Packet & operator>>(sf::Packet& packet, Direction &dir) {
	int dirInt;
	packet >> dirInt;
	dir = static_cast<Direction>(dirInt);
	return packet;
}

sf::Packet & operator<<(sf::Packet& packet, Player& player)
{
	//TODO add EntityCharacteristics
	return packet << player.getID() << player.getPosition().x << player.getPosition().y << player.getSize().x << player.getSize().y << player.getSpeed() << player.getDirection() << player.isMoving() << *player.getCharacteristics();
}

sf::Packet & operator>>(sf::Packet& packet, Player& player)
{
	float x, y, speed;
	float width, height;
	int hp, id;
	bool moving, dead;
	Direction direction;
	PlayerCharacteristics charac;	
	packet >> id >> x >> y >> width >> height >> speed >> direction >> moving >> charac;
	player.setID(id);
	player.setX(x);
	player.setY(y);
	player.setNewX(x);
	player.setNewY(y);
	player.setSize(sf::Vector2f(width, height));
	player.setSpeed(speed);
	player.setDirection(direction);
	player.setMoving(moving);
	player.getCharacteristics()->setHP(charac.getHP());
	player.getCharacteristics()->setDead(charac.isDead());
	return packet;
}

string Player::toString()
{
	string result;
	result += to_string(m_id) + " ";
	result += to_string(m_position.x) + " ";
	result += to_string(m_position.y) + " ";
	result += to_string(m_newPosition.x) + " ";
	result += to_string(m_newPosition.y) + " ";
	result += ((m_moving) ? "moving" : "not_moving") + (string)" ";
	result += m_direction;
	return result;
}
