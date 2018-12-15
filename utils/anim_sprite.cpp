#include "anim_sprite.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

using namespace std;

AnimatedSprite::AnimatedSprite(int numLines, int numFrames, float framerate, std::string texPath, bool playOnce) : m_sprite(), m_numLines(numLines), m_numFrames(numFrames), m_framerate(framerate), m_playOnce(playOnce)
{
    if(!m_tex.loadFromFile(texPath)) {
        cout << "Failed to create AnimatedSprite : wrong texture path ! "<< endl;
    }
    m_frameHeight = m_tex.getSize().y / numLines;
    m_frameWidth = m_tex.getSize().x / numFrames;
    m_sprite.setTexture(m_tex);
    m_sprite.setTextureRect(sf::IntRect(0, 0, m_frameWidth, m_frameHeight));
}

//TODO create init method instead of doing everythin in constructor

void AnimatedSprite::update(float delta)
{
    if(m_timeSinceLastFrame >= m_framerate) {
        if(m_currFrame < m_numFrames - 1) { //if it's not the end of animation
            m_currFrame++; //skip to next frame
        } else {
			if(!m_playOnce) {
				m_currFrame = 0; //if !m_playOnce, restart animation
			}
        }
        m_timeSinceLastFrame = 0.0f; //restart frame time counter
    } else {
        m_timeSinceLastFrame += delta;
    }
    int xpos = m_currFrame * m_frameWidth;
    int ypos = m_currLine * m_frameHeight;
    m_sprite.setTextureRect(sf::IntRect(xpos, ypos, m_frameWidth, m_frameHeight));
}

void AnimatedSprite::draw(sf::RenderWindow& window)
{
	m_sprite.setTexture(m_tex);
    window.draw(m_sprite);
}

void AnimatedSprite::changeLine(int lineNum)
{
   if(lineNum < m_numLines && lineNum >= 0) {
        m_currLine = lineNum;
   }
}

int AnimatedSprite::getLine()
{
    return m_currLine;
}

sf::Sprite& AnimatedSprite::getSprite()
{
    return m_sprite;
}

