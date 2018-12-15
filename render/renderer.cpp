#include "renderer.h"
#include "../game/map.h"
#include "../game/game.h"
#include "../game/player.h"
#include "../utils/anim_sprite.h"
#include <iostream>

using namespace std;

constexpr unsigned int str2int(const char* str, int h = 0)
{
	return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

PlayerRenderer::PlayerRenderer(std::weak_ptr<Player> player, std::shared_ptr<sf::RenderWindow> window) : m_player(player), m_window(window), m_playerAnimSprite(4, 8, 0.05f, std::string("res/player.png"), false), m_playerDeathAnimSprite(1, 6, 0.1f, std::string("res/player_death.png"), true)
{
	if(!m_playerStatic.loadFromFile("res/player_static.png")) {
		cout << "Couldn't load texture player.png from file!" << endl;
	}
}

PlayerRenderer::~PlayerRenderer()
{
}

void PlayerRenderer::render(float delta) {
	if(auto player = m_player.lock()) {
		if(player->isDead()) {
			m_playerDeathAnimSprite.update(delta);
			m_playerDeathAnimSprite.getSprite().setPosition(sf::Vector2f(player->getX(), player->getY()));
			m_playerDeathAnimSprite.draw(*(m_window.get()));
		} else if(!player->isMoving()) {
			sf::Sprite playerSprite;
			playerSprite.setTexture(m_playerStatic);
			switch(str2int(player->getDirection().c_str())) {
				case str2int("left") :
					playerSprite.setTextureRect(sf::IntRect(0, 64, 64, 64));
					break;
				case str2int("right") : 
					playerSprite.setTextureRect(sf::IntRect(0, 192, 64, 64));
					break;
				case str2int("down") :
					playerSprite.setTextureRect(sf::IntRect(0, 128, 64, 64));
					break;
				case str2int("up") :
					playerSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
					break;
			}
			
			playerSprite.setPosition(sf::Vector2f(player->getX(), player->getY()));
			m_window->draw(playerSprite);
		} else { 
			switch(str2int(player->getDirection().c_str())) {
				case str2int("left") :
					m_playerAnimSprite.changeLine(1);
					break;
				case str2int("right") : 
					m_playerAnimSprite.changeLine(3);
					break;
				case str2int("down") :
					m_playerAnimSprite.changeLine(2);
					break;
				case str2int("up") :
					m_playerAnimSprite.changeLine(0);
					break;
			}
			m_playerAnimSprite.update(delta);
			m_playerAnimSprite.getSprite().setPosition(sf::Vector2f(player->getX(), player->getY()));
			m_playerAnimSprite.draw(*m_window);
		}
	} else {
		cout << "couldn't lock player : no such player" << endl;
	}
}

Renderer::Renderer(int localID, std::weak_ptr<Game> game, std::shared_ptr<sf::RenderWindow> window) : m_game(game), m_window(window), m_bombAnim(1, 2, 0.1f, "res/bomb.png", false), m_bombExplAnim(1, 2, 0.1f, "res/bomb_explosion.png", false), localID(localID)
{

}

int Renderer::init()
{
	if(loadTextures() != 0) {
		cout << "Some textures failed to load ! " << endl;
		return -1;
	}
	if(!m_font.loadFromFile("res/DejaVuSans.ttf")) {
		cout << "Failed to load font DejaVuSans !" << endl;
		return -2;
	}
	m_text.setFont(m_font);
	m_text.setCharacterSize(14);
	m_text.setFillColor(sf::Color::Red);
	if(auto game = m_game.lock()) {
		for(auto& p : game->getEntities()) {
			addPlayerRenderer(p.first);
		}
	} else {
		return -3;
	}
	return 0;
}

Renderer::~Renderer()
{
	cout << "Renderer destroyed" << endl;
}

int Renderer::loadTileTexture(std::string id)
{
    sf::Texture tex;
    if(!tex.loadFromFile("res/" + id + ".png")) {
        cout << "failed to load .png from file. Aborting. " << endl;
        return -1;
    }
    tileTextures[id] = tex;
    return 0;
}

int Renderer::loadEntityTexture(std::string id)
{
	sf::Texture tex;
	if(!tex.loadFromFile("res/" + id + ".png")) {
		cout << "failed to load .png from file. Aborting. " << endl;
		return -1;
	}
	
	entityTextures[id] = tex;
	return 0;
}

int Renderer::loadTextures() {
	int result = loadTileTexture("stone");
	result += loadTileTexture("wall");
	result += loadTileTexture("breakable");
	result += loadEntityTexture("bomb");
	cout << "loaded textures" << endl;
	return result;
}

void Renderer::render(float delta)
{
	if(auto game = m_game.lock()) {
		sf::Sprite tileSprite;
		tileSprite.setScale(0.5f, 0.5f);
		for(vector<shared_ptr<Tile>> line : game->getTerrain().lock()->getTiles()) {
			for(shared_ptr<Tile> tile : line) {
				tileSprite.setTexture(tileTextures[tile->getType()]);
				tileSprite.setPosition(sf::Vector2f(tile->getX(), tile->getY()));
				
				m_window->draw(tileSprite);
			}
		}
		
		//update bomb anims
		m_bombAnim.update(delta);
		m_bombExplAnim.update(delta);
		
		for(std::map<int, bomb_ptr_t>::iterator it = game->getBombs().begin(); it != game->getBombs().end(); it++) {
			if(!it->second->hasExploded()) {
				m_bombAnim.getSprite().setPosition(sf::Vector2f(it->second->getX(), it->second->getY()));
				m_bombAnim.getSprite().setScale(2.0f, 2.0f);
				m_bombAnim.draw(*(m_window.get()));
			} else {
				m_bombExplAnim.getSprite().setScale(2.0f, 2.0f);
				//offset of 32 here is for the animation to be rightly placed
				m_bombExplAnim.getSprite().setPosition(sf::Vector2f(it->second->getX()-32, it->second->getY()-32));
				m_bombExplAnim.draw(*(m_window.get()));
			}
		}
		
		for(std::map<int, PlayerRenderer>::iterator it = m_playerRenderers.begin(); it != m_playerRenderers.end(); it++) {
			it->second.render(delta);
		}
		
		if(auto localPlayer = game->getEntity(localID).lock()) {
			std::string textContent = "HP : " + to_string(localPlayer->getHP());
			m_text.setString(textContent);
			m_text.setPosition(0, 0);
			m_window->draw(m_text);
			
			if(localPlayer->isDead()) {
				m_text.setString("You are dead !");
				m_text.setPosition(400, 304);
				m_window->draw(m_text);
			}
		}
	} else {
		cout << "couldn't lock game" << endl;
	}
}

void Renderer::onNotify(int objectID, Subject *sub, ::Event ev, sf::Uint64 timestamp)
{
	if(ev == EVENT_PLAYER_JOIN) {
		if(auto game = m_game.lock()) {
			m_playerRenderers.insert(std::pair<int, PlayerRenderer>(objectID, PlayerRenderer(game->getEntity(objectID), m_window)));
		}
	} else if(ev == EVENT_PLAYER_PART) {
		m_playerRenderers.erase(objectID);
	}
}

