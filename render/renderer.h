
#ifndef RENDER_RENDERER_H_
#define RENDER_RENDERER_H_

#include <memory>
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

#include "../game/game.h"
#include "../utils/anim_sprite.h"
#include "../utils/observer.h"

class Renderer;

class PlayerRenderer {
	public:
		PlayerRenderer(std::weak_ptr<Player> player, std::shared_ptr<sf::RenderWindow> window);
		~PlayerRenderer();
		void render(float delta);
	private:
		std::weak_ptr<Player> m_player;
		std::shared_ptr<sf::RenderWindow> m_window;
		AnimatedSprite m_playerAnimSprite;
		AnimatedSprite m_playerDeathAnimSprite;
		sf::Texture m_playerStatic;
};


class Renderer : public Observer {
	public:
		Renderer(int localID, std::weak_ptr<Game> game, std::shared_ptr<sf::RenderWindow> window);
		int init();
		~Renderer();
        int loadTextures();
		int loadTileTexture(std::string id);
        int loadEntityTexture(std::string id);
		void render(float delta);
		sf::Texture getTileTexture(std::string id) { return tileTextures[id]; }
		sf::Texture getEntityTexture(std::string id) { return entityTextures[id]; }
		virtual void onNotify(int objectID, Subject *sub, Event ev, sf::Uint64 timestamp);
		void addPlayerRenderer(int id) { m_playerRenderers.insert(std::pair<int, PlayerRenderer>(id, PlayerRenderer(m_game.lock()->getEntity(id), m_window))); }
	private:
		std::weak_ptr<Game> m_game;
		std::weak_ptr<Player> m_player;
        std::shared_ptr<sf::RenderWindow> m_window;
        std::map<std::string, sf::Texture> tileTextures;
        std::map<std::string, sf::Texture> entityTextures;
		std::map<int, PlayerRenderer> m_playerRenderers;
		AnimatedSprite m_bombAnim;
		AnimatedSprite m_bombExplAnim;
		sf::Font m_font;
		sf::Text m_text;
		int localID;
};

#endif /* RENDER_RENDERER_H_ */
