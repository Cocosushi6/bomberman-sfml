#ifndef SRC_UTILS_H
#define SRC_UTILS_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

const int TILE_SIZE = 32;
extern bool hasFocus;

struct InputState {
	bool state;
	int key;
	sf::Uint64 timestamp;
	float delta;
};

template <typename T>
sf::Vector2<T> toTileCoordinates(T x, T y) {
	return sf::Vector2<T>(x / TILE_SIZE, y / TILE_SIZE);
}

sf::Uint64 getTimestamp();

InputState stateForKey(sf::Keyboard::Key key, float delta, sf::Uint64 timestamp = 0);

sf::Packet& operator<<(sf::Packet& packet, InputState &state);
sf::Packet& operator>>(sf::Packet& packet, InputState &state);
std::ostream& operator<<(std::ostream& stream, InputState &state);

#endif
