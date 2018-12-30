#ifndef SRC_UTILS_H
#define SRC_UTILS_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <functional>
#include <iostream>

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
sf::Packet& operator<<(sf::Packet& packet, sf::Vector2f &vector);
sf::Packet& operator>>(sf::Packet& packet, sf::Vector2f &vector);
std::ostream& operator<<(std::ostream& stream, InputState &state);

template<typename T>
void prompt(std::string message, T& outputValue, std::function<bool(T)> validInput) {
	T tempValue;

	do {
		std::cout << message << std::endl;
		std::cin >> tempValue;
	} while(!validInput(tempValue));

	outputValue = tempValue;
}



#endif
