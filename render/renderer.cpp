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

PlayerRenderer::PlayerRenderer(Player* player, sf::RenderWindow* window) : m_player(player), m_window(window), m_playerAnimSprite(4, 8, 0.05f, string("res/player.png"), false), m_playerDeathAnimSprite(1, 6, 0.1f, string("res/player_death.png"), true)
{
	if(!m_playerStatic.loadFromFile("res/player_static.png")) {
		cout << "Couldn't load texture player.png from file!" << endl;
	}
}

PlayerRenderer::~PlayerRenderer()
{
}

void PlayerRenderer::render(float delta) {
	if(m_player == nullptr) { cout << "Player renderer has no player!"; }

	if(m_player->getCharacteristics()->isDead()) {
		m_playerDeathAnimSprite.update(delta);
		m_playerDeathAnimSprite.getSprite().setPosition(m_player->getPosition());
		m_playerDeathAnimSprite.draw(*m_window);
	} else if(!m_player->isMoving()) {
		sf::Sprite playerSprite;
		playerSprite.setTexture(m_playerStatic);
		switch(m_player->getDirection()) {
			case LEFT :
				playerSprite.setTextureRect(sf::IntRect(0, 64, 64, 64));
				break;
			case RIGHT : 
				playerSprite.setTextureRect(sf::IntRect(0, 192, 64, 64));
				break;
			case DOWN : 
				playerSprite.setTextureRect(sf::IntRect(0, 128, 64, 64));
				break;
			case UP :
				playerSprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
				break;
		}
		
		playerSprite.setPosition(m_player->getPosition());
		m_window->draw(playerSprite);
	} else { 
		switch(m_player->getDirection()) {
			case LEFT :
				m_playerAnimSprite.changeLine(1);
				break;
			case RIGHT : 
				m_playerAnimSprite.changeLine(3);
				break;
			case DOWN :  
				m_playerAnimSprite.changeLine(2);
				break;
			case UP :
				m_playerAnimSprite.changeLine(0);
				break;
		}
		m_playerAnimSprite.update(delta);
		m_playerAnimSprite.getSprite().setPosition(m_player->getPosition());
		m_playerAnimSprite.draw(*m_window);
	}
}

Renderer::Renderer(int localID, Game* game, unique_ptr<sf::RenderWindow> window) : m_game(game), m_window(move(window)), m_bombAnim(1, 2, 0.1f, "res/bomb.png", false), m_bombExplAnim(1, 2, 0.1f, "res/bomb_explosion.png", false), localID(localID)
{
	m_player = dynamic_cast<Player*>(m_game->getEntity(localID));
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
	if(m_game == nullptr) {
		return -3;
	}
	m_text.setFont(m_font);
	m_text.setCharacterSize(14);
	m_text.setFillColor(sf::Color::Red);
	for(auto& p : m_game->getEntities()) {
		addPlayerRenderer(p.first);
	}
	return 0;
}

Renderer::~Renderer()
{
	cout << "Renderer destroyed" << endl;
}

int Renderer::loadTileTexture(string id)
{
    sf::Texture tex;
    if(!tex.loadFromFile("res/" + id + ".png")) {
        cout << "failed to load .png from file. Aborting. " << endl;
        return -1;
    }
    tileTextures[id] = tex;
    return 0;
}

int Renderer::loadEntityTexture(string id)
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
	if(m_game == nullptr) { cout << "Renderer has no game attached ! " << endl; }

	sf::Sprite tileSprite;
	tileSprite.setScale(0.5f, 0.5f);
	for(vector<Tile*> line : m_game->getTerrain()->getTiles()) {
		for(auto&& tile : line) {
			tileSprite.setTexture(tileTextures[tile->getType()]);
			tileSprite.setPosition(sf::Vector2f(tile->getX(), tile->getY()));
			
			m_window->draw(tileSprite);
		}
	}
	
	//update bomb anims
	m_bombAnim.update(delta);
	m_bombExplAnim.update(delta);
	
	auto gameBombs = m_game->getBombs();
	for(auto it = gameBombs.begin(); it != gameBombs.end(); it++) {
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
	
	for(map<int, PlayerRenderer>::iterator it = m_playerRenderers.begin(); it != m_playerRenderers.end(); it++) {
		it->second.render(delta);
	}
	
	string textContent = "HP : " + to_string(m_player->getCharacteristics()->getHP());
	m_text.setString(textContent);
	m_text.setPosition(0, 0);
	m_window->draw(m_text);
	
	if(m_player->getCharacteristics()->isDead()) {
		m_text.setString("You are dead !");
		m_text.setPosition(400, 304);
		m_window->draw(m_text);
	}
}

void Renderer::onNotify(int objectID, Subject *sub, ::Event ev, sf::Uint64 timestamp)
{
	if(ev == EVENT_PLAYER_JOIN) {
		if(m_game != nullptr) {
			addPlayerRenderer(objectID);
		}
	} else if(ev == EVENT_PLAYER_PART) {
		m_playerRenderers.erase(objectID);
	}
}

