#include "player.h"
#include "game.h"
#include "../utils/utils.h"
#include "../utils/player_manag.h"
#include <SFML/Graphics.hpp>

using namespace std;

constexpr unsigned int str2int(const char* str, int h = 0)
{
	return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

Player::Player(float x, float y, Game* game, int id, PlayerManager* manager) : m_x(x), m_y(y), m_newX(m_x), m_newY(m_y), m_game(game), m_id(id), m_pManager(manager), m_hp(100), m_speed(200.0f), m_moving(false), m_direction("down"), m_dead(false) {
    
}

Player::Player() : m_x(-1.0f), m_y(-1.0f), m_newX(m_x), m_newY(m_y), m_game(nullptr), m_id(-1), m_pManager(nullptr), m_hp(100), m_speed(200.0f), m_moving(false), m_direction("down"), m_dead(false) 
{
}

Player::~Player()
{
}

sf::FloatRect Player::getBounds() {
	return sf::FloatRect(m_x+10, m_y+10, SPRITE_SIZE-20, SPRITE_SIZE-10);
}

sf::Vector2<float> Player::getTileCoordinates()
{
	return toTileCoordinates(m_x, m_y);
}

void Player::giveDamage(int amount)
{
	cout << "Received damage ! " << endl;
	m_hp -= amount;
	if(m_hp <= 0) {
		m_hp = 0;
		m_dead = true;
	}
}

int Player::checkCollision(float newX, float newY)
{
	if (m_game != nullptr)
	{
		//TODO check collision with other players too
		sf::FloatRect nextBounds(this->getBounds());
		switch (str2int(m_direction.c_str()))
		{
		case str2int("left"):
			nextBounds.left += newX;
			break;
		case str2int("right"):
			nextBounds.width += newX;
			break;
		case str2int("up"):
			nextBounds.top += newY;
			break;
		case str2int("down"):
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

void Player::udpate()
{
	float velocity = m_lastDelta * m_speed;
	if(m_x > m_newX) {
		if(m_x - m_newX > velocity) {
			m_x -= velocity;
		} else {
			m_x -= m_x - m_newX;
		}
	} else {
		if(m_newX - m_x > velocity) {
			m_x += velocity;
		} else {
			m_x += m_newX - m_x;
		}
	}
	
	if(m_y > m_newY) {
		if(m_y - m_newY > velocity) {
			m_y -= velocity;
		} else {
			m_y -= m_y - m_newY;
		}
	} else {
		if(m_newY - m_y > velocity) {
			m_y += velocity;
		} else {
			m_y += m_newY - m_y;
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
	
	this->m_newX = newX + m_x;
	this->m_newY = newY + m_y;
	m_lastDelta = delta;
}

int Player::getHP() { 
	return m_hp; 
}


sf::Packet & operator<<(sf::Packet& packet, Player& player)
{
	return packet << player.getID() << player.getX() << player.getY() << player.getSpeed() << player.getHP() << player.getDirection() << player.isMoving() << player.isDead();
}

sf::Packet & operator>>(sf::Packet& packet, Player& player)
{
	float x, y, speed;
	int hp, id;
	bool moving, dead;
	string direction;
	packet >> id >> x >> y >> speed >> hp >> direction >> moving >> dead;
	player.setID(id);
	player.setX(x);
	player.setY(y);
	player.setNewX(x);
	player.setNewY(y);
	player.setSpeed(speed);
	player.setLife(hp);
	player.setDirection(direction);
	player.setMoving(moving);
	player.setDead(dead);
	return packet;
}

string Player::toString()
{
	string result;
	result += to_string(m_id) + " ";
	result += to_string(m_x) + " ";
	result += to_string(m_y) + " ";
	result += to_string(m_newX) + " ";
	result += to_string(m_newY) + " ";
	result += ((m_moving) ? "moving" : "not_moving") + (string)" ";
	result += m_direction;
	return result;
}
