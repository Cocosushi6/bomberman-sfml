#ifndef GAME_MAP_H_
#define GAME_MAP_H_

#include <vector>
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include "../utils/subject.h"
#include "../utils/utils.h"

class Terrain;

class Tile {
	
    public:
        Tile(int x, int y, int id, std::string type, bool solid, bool breakable);
		Tile();
        ~Tile();
        int getX() { return m_x; }
        int getY() { return m_y; }
        int getID() { return m_id; }
        std::string getType() { return m_type; }
        bool isSolid() { return m_solid; }
        bool isBreakable() { return m_breakable; }
        sf::FloatRect getBounds() { return sf::FloatRect(m_x, m_y, TILE_SIZE, TILE_SIZE);}
        sf::Vector2i getWorldCoordinates() { return sf::Vector2i(m_x / TILE_SIZE, m_y / TILE_SIZE); }
		void setX(int x) { m_x = x; }
		void setY(int y) { m_y = y; }
		void setID(int id) { m_id = id; }
		void setType(std::string type) { m_type = type; }
		void setSolid(bool solid) { m_solid = solid; }
		void setBreakable(bool breakable) { m_breakable = breakable; }
    private:
        int m_x, m_y;
        int m_id;
        std::string m_type;
		bool m_solid = false;
		bool m_breakable = false;
};

typedef std::shared_ptr<Tile> tile_ptr_t;

class Terrain : public Subject {
	public:
        Terrain();
        ~Terrain();
		std::shared_ptr<Tile> getTileAt(int x, int y);
        std::vector<std::vector<tile_ptr_t>> getTiles() { return m_tiles; }
        void setTiles(std::vector<std::vector<tile_ptr_t>> tiles) { m_tiles = tiles; }
        void breakTileAt(int worldX, int worldY, std::string newTileType);
		void printWorld();
	private:
        int generateTiles();
        std::vector<std::vector<tile_ptr_t>> m_tiles;
};

sf::Packet& operator<<(sf::Packet& packet, Tile &t);
sf::Packet& operator>>(sf::Packet& packet, Tile &t);
sf::Packet& operator<<(sf::Packet& packet, Terrain &terrain);
sf::Packet& operator>>(sf::Packet& packet, Terrain &terrain);

#endif /* GAME_MAP_H_ */
