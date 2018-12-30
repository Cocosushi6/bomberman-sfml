#include "characteristics.h"

EntityCharacteristics::EntityCharacteristics(unsigned int hp, bool dead) : m_hp(hp), m_dead(dead) 
{}

EntityCharacteristics::EntityCharacteristics() : m_hp(0), m_dead(false)
{}

