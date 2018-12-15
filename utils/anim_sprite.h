#ifndef SRC_ANIM_SPRITE_H
#define SRC_ANIM_SPRITE_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <string>
#include <iostream>
#include <memory>

class AnimatedSprite { //TODO think about making AnimatedSprite extend sprite ?
    public:
        AnimatedSprite(int numLines, int numFrames, float framerate, std::string texPath, bool playOnce);
        void update(float delta);
        void draw(sf::RenderWindow &window);
        int getLine();
        void changeLine(int lineNum);
        sf::Sprite& getSprite();
    private:
        sf::Sprite m_sprite;
        sf::Texture m_tex;
        int m_numLines, m_numFrames;
        int m_frameWidth, m_frameHeight;
        float m_framerate;
        int m_currLine = 0, m_currFrame = 0;
        float m_timeSinceLastFrame = 0.0f;
		bool m_playOnce = false;
};

#endif /* SRC_ANIM_SPRITE_H */
