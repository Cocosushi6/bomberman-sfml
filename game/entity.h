#ifndef SRC_ENTITY_H
#define SRC_ENTITY_H

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

class Game;

enum Direction {
    LEFT = 0,
    RIGHT = 1,
    UP = 2,
    DOWN = 3
};

//pure virtual class
class Entity {
    public:
        virtual ~Entity() = default;
        Entity(const Entity& other) = default;

        virtual void move(float newX, float newY, float delta) = 0;
        virtual void update() = 0;
       
        //utility methods
        sf::FloatRect getBounds();
        sf::Vector2f getTileCoordinates();
        virtual std::string toString() = 0;

        //getters
        sf::Vector2f getPosition() { return m_position; }
        sf::Vector2f getNewPosition() { return m_newPosition; }
        sf::Vector2f getSize() { return m_size; }
		float getSpeed() { return m_speed; }
        float getLastDelta() { return m_lastDelta; }
		bool isMoving() { return m_moving; }
		Direction getDirection() { return m_direction; }
        int getID() { return m_id; }

        //setters
        void setPosition(sf::Vector2f pos) { m_position = pos; }
        void setNewPosition(sf::Vector2f pos) { m_newPosition = pos; }
        void setSize(sf::Vector2f size) { m_size = size; }
        void setX(float x) { m_position.x = x; }
        void setY(float y) { m_position.y = y; }
        void setNewX(float x) { m_newPosition.x = x; }
        void setNewY(float y) { m_newPosition.y = y; }
		void setSpeed(float speed) { m_speed = speed; }
		void setMoving(bool moving) { m_moving = moving; }
		void setDirection(Direction direction) { m_direction = direction; }
        void setID(int id) { m_id = id; }
        void setGame(Game* game) { m_game = game; }
        void setLastDelta(float lastDelta) { m_lastDelta = lastDelta; }

    protected:
        Entity(float x, float y, float width, float height, Game* game, int id);
        Entity(sf::Vector2f position, sf::Vector2f size, Game* game, int id);
        Entity();
        virtual int checkCollision(float newX, float newY) = 0;

    
        sf::Vector2f m_position, m_newPosition, m_size;
        float m_speed;
        float m_lastDelta;
        bool m_moving;
        Direction m_direction = DOWN;

        int m_id;
        Game *m_game = nullptr;
};


#endif // SRC_ENTITY_H