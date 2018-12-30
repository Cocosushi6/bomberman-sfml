#include "player_characteristics.h"
#include <iostream>

using namespace std;

PlayerCharacteristics::PlayerCharacteristics(unsigned int hp, bool dead) : EntityCharacteristics(hp, dead)
{}

PlayerCharacteristics::PlayerCharacteristics() : EntityCharacteristics() 
{}

void PlayerCharacteristics::giveDamage(int amount) {
	cout << "Received damage ! " << endl;
	m_hp -= amount;
	if(m_hp <= 0) {
		m_hp = 0;
		m_dead = true;
	}
}

sf::Packet& operator<<(sf::Packet& packet, PlayerCharacteristics& charac) {
	return packet << charac.getHP() << charac.isDead();
}

sf::Packet& operator>>(sf::Packet& packet, PlayerCharacteristics& charac) {
	unsigned int hp;
	bool dead;
	packet >> hp >> dead;
	charac.setHP(hp);
	charac.setDead(dead);
	return packet;	
}