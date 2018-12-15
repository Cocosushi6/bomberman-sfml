#include "map.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "../utils/utils.h"

using namespace std;

Tile::Tile(int x, int y, int id, std::string type, bool solid, bool breakable) : m_x(x), m_y(y), m_id(id), m_type(type), m_solid(solid), m_breakable(breakable) {
    
}

Tile::Tile() {}

Tile::~Tile()
{
}

Terrain::Terrain()
{
    if(generateTiles() != 0) {
        cout << "Failed to build terrain : can't load tiles. Aborting. " << endl;
        return;
    }
}

Terrain::~Terrain()
{
}

std::shared_ptr<Tile> Terrain::getTileAt(int x, int y) {
	int tileX = x / TILE_SIZE;
	int tileY = y / TILE_SIZE;
	try {
		std::shared_ptr<Tile> result = m_tiles.at(tileY).at(tileX);
		return result;
	} catch(std::out_of_range ex) {
		cerr << "Tile not in bounds  : " << x << ", " << y << " !" << endl;
		cerr << "TileX : " << tileX << ", TileY " << tileY << endl; 
		return nullptr;
	}
}

void Terrain::printWorld() {
	for(std::vector<std::shared_ptr<Tile>> line : m_tiles) {
		for(std::shared_ptr<Tile> t : line) {
			if(t->getType() == "stone") {
				cout << "S";
			} else if(t->getType() == "wall") {
				cout << "W";
			} else if(t->getType() == "breakable") {
				cout << "B";
			}
		}
		cout << endl;
	}
	
}

void Terrain::breakTileAt(int worldX, int worldY, std::string newTileType)
{
	std::shared_ptr<Tile> tileAt = m_tiles.at(worldY).at(worldX);
	if(tileAt->isBreakable()) {
		shared_ptr<Tile> newTile = std::make_shared<Tile>(worldX * TILE_SIZE, worldY * TILE_SIZE, tileAt->getID(), newTileType, false, false);
		m_tiles[worldY][worldX] = newTile;
		notify(newTile->getID(), EVENT_TILE_CHANGE);
	}
}

int Terrain::generateTiles()
{
    cout << "loading tiles..." << endl;
    std::ifstream mapFile("res/map.txt");
    if(!mapFile) {
        cout << "Error while reading map file, things will get ugly sooooon around here" << endl;
        return -2;
    }
    
    string line;
    int y = 0;
    while(getline(mapFile, line)) {
       vector<std::shared_ptr<Tile>> tileLine;
       for(int i = 0; i < line.length(); i++) {
            switch(line[i]) {
                case 'W' : 
                {
                    shared_ptr<Tile> tileW = std::shared_ptr<Tile>(new Tile(i * TILE_SIZE, y * TILE_SIZE, i + line.length() * y, "wall", true, false));
                    tileLine.push_back(tileW);
                    break;
                }
                case 'S' :
				{
                    shared_ptr<Tile> tileS = std::shared_ptr<Tile>(new Tile(i * TILE_SIZE, y * TILE_SIZE, i+line.length() * y, "stone", false, false));
                    tileLine.push_back(tileS);
                    break;
				}
				case 'B' :
				{
					shared_ptr<Tile> tileB = std::shared_ptr<Tile>(new Tile(i * TILE_SIZE, y * TILE_SIZE, i+line.length() * y, "breakable", true, true));
					tileLine.push_back(tileB);
					break;
				}
            }
       }
       m_tiles.push_back(tileLine);
       y++;
    }
    cout << "tiles loaded !" << endl;
    return 0;
}

sf::Packet& operator<<(sf::Packet& packet, Tile &t) {
	return packet << t.getX() << t.getY() << t.getID() << t.getType() << t.isSolid() << t.isBreakable();
}

sf::Packet& operator>>(sf::Packet& packet, Tile &t) {
	int x, y, id;	
	bool solid, breakable;
	std::string type;
	packet >> x >> y >> id >> type >> solid >> breakable;
	t.setX(x);
	t.setY(y);
	t.setID(id);
	t.setType(type);
	t.setSolid(solid);
	t.setBreakable(breakable);
	return packet;
}

sf::Packet& operator<<(sf::Packet& packet, Terrain &terrain) {
	sf::Uint16 mapHeight = terrain.getTiles().size();
	packet << mapHeight;
	for(std::vector<tile_ptr_t> line : terrain.getTiles()) {
		sf::Uint16 lineSize = line.size();
		packet << lineSize;
		for(tile_ptr_t tile : line) {
			packet << *tile;
		}
	}
	cout << "Tiles sent" << endl;
	return packet;
}

sf::Packet& operator>>(sf::Packet& packet, Terrain &terrain) {
	std::vector<std::vector<tile_ptr_t>> tiles;
	sf::Uint16 mapHeight;
	packet >> mapHeight;
	for(int i = 0; i < mapHeight; i++) {
		sf::Uint16 lineSize;
		packet >> lineSize;
		std::vector<tile_ptr_t> line;
		for(int j = 0; j < lineSize; j++) {
			tile_ptr_t t = std::make_shared<Tile>();
			packet >> *t;
			line.push_back(t);
		}
		tiles.push_back(line);
	}
	terrain.setTiles(tiles);
	cout << "Tiles received" << endl;
	return packet;
}
