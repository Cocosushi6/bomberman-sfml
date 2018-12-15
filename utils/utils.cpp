
#include "utils.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <chrono>


sf::Uint64 getTimestamp()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); //err... correct, but see doc if you want info 'bout this
}

InputState stateForKey(sf::Keyboard::Key key, float delta, sf::Uint64 timestamp)
{
	InputState state;
	if(sf::Keyboard::isKeyPressed(key)) {
		state.state = true;
	} else {
		state.state = false;
	}
	state.key = key;
	state.timestamp = (timestamp == 0) ? getTimestamp() : timestamp; 
	state.delta = delta;
	return state;
}


sf::Packet & operator>>(sf::Packet& packet, InputState& state)
{
	return packet >> state.state >> state.key >> state.timestamp >> state.delta;
}

sf::Packet & operator<<(sf::Packet& packet, InputState& state)
{
	return packet << state.state << state.key << state.timestamp << state.delta;
}

std::ostream& operator<<(std::ostream& stream, InputState& state) {
	return stream << state.delta << ", " << state.key << ", " << state.state << ", " << state.timestamp;
}



