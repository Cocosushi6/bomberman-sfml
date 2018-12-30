#include "entity.h"
#include <memory>
#include <iostream>
#include "../utils/utils.h"

using namespace std;

Entity::Entity(sf::Vector2f position, sf::Vector2f size, Game *game, int id) : m_position(position), m_newPosition(position), m_size(size), m_game(game), m_id(id), m_speed(200.0f), m_moving(false), m_direction(DOWN) {
}

Entity::Entity(float x, float y, float width, float height, Game *game, int id) : Entity(sf::Vector2f(x, y), sf::Vector2f(width, height), game, id) 
{}

Entity::Entity() : m_position(-1.0f, -1.0f), m_newPosition(m_position), m_size(0.0f, 0.0f), m_moving(false), m_direction(DOWN), m_id(-1), m_game(nullptr), m_lastDelta(0.0f)
{
}


sf::FloatRect Entity::getBounds() {
	return sf::FloatRect(m_position.x+10, m_position.y+10, m_size.x-20, m_size.y-10);
}

sf::Vector2<float> Entity::getTileCoordinates()
{
	return toTileCoordinates(m_position.x, m_position.y);
}