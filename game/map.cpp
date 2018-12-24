#include "map.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "../utils/utils.h"

using namespace std;

Tile::Tile(int x, int y, int id, string type, bool solid, bool breakable) : m_x(x), m_y(y), m_id(id), m_type(type), m_solid(solid), m_breakable(breakable) {
    
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

Tile* Terrain::getTileAt(int x, int y) {
	int tileX = x / TILE_SIZE;
	int tileY = y / TILE_SIZE;
	try {
		Tile* result = m_tiles.at(tileY).at(tileX).get();
		return result;
	} catch(out_of_range ex) {
		cerr << "Tile not in bounds  : " << x << ", " << y << " !" << endl;
		cerr << "TileX : " << tileX << ", TileY " << tileY << endl; 
		return nullptr;
	}
}

void Terrain::printWorld() {
	for(int i = 0; i < m_tiles.size(); i++) {
		for(int j = 0; j < m_tiles[0].size(); j++) {
			Tile* t = m_tiles[i][j].get();
			if(t->getType() == "stone") {
				cout << "S";
			} else if(t->getType() == "wall") {
				cout << "W";
			} else if(t->getType() == "breakable") {
				cout << "B";
			}
		}
	}
}

void Terrain::breakTileAt(int worldX, int worldY, string newTileType)
{
	Tile* tileAt = m_tiles.at(worldY).at(worldX).get();
	if(tileAt->isBreakable()) {
		unique_ptr<Tile> newTile = make_unique<Tile>(worldX * TILE_SIZE, worldY * TILE_SIZE, tileAt->getID(), newTileType, false, false);
		notify(newTile->getID(), EVENT_TILE_CHANGE);
		m_tiles[worldY][worldX] = move(newTile);
	}
}

int Terrain::generateTiles()
{
    cout << "loading tiles..." << endl;
    ifstream mapFile("res/map.txt");
    if(!mapFile) {
        cout << "Error while reading map file, things will get ugly sooooon around here" << endl;
        return -2;
    }
    
    string line;
    int y = 0;
    while(getline(mapFile, line)) {
       vector<tile_ptr_t> tileLine;
       for(int i = 0; i < line.length(); i++) {
            switch(line[i]) {
                case 'W' : 
                {
                    unique_ptr<Tile> tileW = make_unique<Tile>(i * TILE_SIZE, y * TILE_SIZE, i + line.length() * y, "wall", true, false);
                    tileLine.push_back(move(tileW));
                    break;
                }
                case 'S' :
				{
                    unique_ptr<Tile> tileS = make_unique<Tile>(i * TILE_SIZE, y * TILE_SIZE, i+line.length() * y, "stone", false, false);
                    tileLine.push_back(move(tileS));
                    break;
				}
				case 'B' :
				{
					unique_ptr<Tile> tileB = make_unique<Tile>(i * TILE_SIZE, y * TILE_SIZE, i+line.length() * y, "breakable", true, true);
					tileLine.push_back(move(tileB));
					break;
				}
            }
       }
       m_tiles.push_back(move(tileLine));
       y++;
    }
    cout << "tiles loaded !" << endl;
    return 0;
}

void Terrain::setTiles(vector<vector<tile_ptr_t>> tiles) {
	m_tiles = move(tiles);
}

vector<vector<Tile*>> Terrain::getTiles() {
	vector<vector<Tile*>> returnVec(m_tiles.size(), vector<Tile*>(m_tiles[0].size()));
	for(int i = 0; i < m_tiles.size(); i++) {
		for(int j = 0; j < m_tiles[0].size(); j++) {
			returnVec[i][j] = m_tiles[i][j].get();
		}
	}
	return returnVec;
}

sf::Packet& operator<<(sf::Packet& packet, Tile &t) {
	return packet << t.getX() << t.getY() << t.getID() << t.getType() << t.isSolid() << t.isBreakable();
}

sf::Packet& operator>>(sf::Packet& packet, Tile &t) {
	int x, y, id;	
	bool solid, breakable;
	string type;
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
	for(vector<Tile*> line : terrain.getTiles()) {
		sf::Uint16 lineSize = line.size();
		packet << lineSize;
		for(Tile* tile : line) {
			packet << *tile;
		}
	}
	cout << "Tiles sent" << endl;
	return packet;
}

sf::Packet& operator>>(sf::Packet& packet, Terrain &terrain) {
	vector<vector<tile_ptr_t>> tiles;
	sf::Uint16 mapHeight;
	packet >> mapHeight;
	for(int i = 0; i < mapHeight; i++) {
		sf::Uint16 lineSize;
		packet >> lineSize;
		vector<tile_ptr_t> line;
		for(int j = 0; j < lineSize; j++) {
			tile_ptr_t t = make_unique<Tile>();
			packet >> *t;
			line.push_back(move(t));
		}
		tiles.push_back(move(line));
	}
	terrain.setTiles(move(tiles));
	cout << "Tiles received" << endl;
	return packet;
}
