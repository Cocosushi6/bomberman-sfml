#ifndef PLAYER_CHARAC_H
#define PLAYER_CHARAC_H

#include "characteristics.h"
#include <SFML/Network.hpp>

class PlayerCharacteristics : public EntityCharacteristics {
    public:
        PlayerCharacteristics(unsigned int hp, bool dead);
        PlayerCharacteristics();
        ~PlayerCharacteristics() = default;

        virtual void giveDamage(int amount);
};

sf::Packet& operator<<(sf::Packet& packet, PlayerCharacteristics& charac);
sf::Packet& operator>>(sf::Packet& packet, PlayerCharacteristics& charac);

#endif