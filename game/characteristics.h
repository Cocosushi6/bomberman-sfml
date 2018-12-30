#ifndef SRC_ENTITY_DATA_H
#define SRC_ENTITY_DATA_H

//pure virtual class
class EntityCharacteristics {
    public:
        EntityCharacteristics(unsigned int hp, bool dead);
        EntityCharacteristics();
        ~EntityCharacteristics() = default;
        
        virtual void giveDamage(int amount) = 0;

        void setHP(unsigned int hp) { m_hp = hp; }
        void setDead(bool dead) { m_dead = dead; }

        unsigned int getHP() { return m_hp; }
        bool isDead() { return m_dead; }

    protected:
        unsigned int m_hp;
        bool m_dead;
};

#endif