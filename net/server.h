#ifndef SRC_SERVER_H
#define SRC_SERVER_H

#include <memory>
#include <string>
#include <SFML/Network.hpp>

#include "../utils/subject.h"
#include "../utils/observer.h"
#include "../game/game.h"

class RemoteClient;

typedef std::shared_ptr<RemoteClient> rclient_ptr_t;
typedef std::shared_ptr<sf::TcpSocket> tcp_sock_ptr_t;

class Server : public Subject, public Observer {
	public:
		Server(unsigned int tcpPort, unsigned int udpPort, std::weak_ptr<Game> game);
		int init();
			
		//net management
		sf::Packet& getUDPDataPacket() { return m_udpDataPacket; }
		sf::Packet& getTCPDataPacket() { return m_tcpDataPacket; }

		void poll(float delta);
		
		//packet parsing
		int parsePacket(sf::Packet packet, sf::IpAddress address);
		
		//client management
		void addClient(int id,  rclient_ptr_t client);
		void removeClient(int id);
		std::weak_ptr<RemoteClient> getClient(int id);
		virtual void onNotify(int objectID, Subject *sub, Event ev, sf::Uint64 timestamp);
	private:
		int sendTCP(int clientID, sf::Packet packet);
		int sendUDP(int clientID, sf::Packet packet);
		void sendTCPToAll(sf::Packet packet);
		void sendUDPToAll(sf::Packet packet);
		
		std::map<int, rclient_ptr_t> m_clients;
		
		sf::SocketSelector m_sockSelector;
		sf::UdpSocket m_udpSocket;
		sf::TcpListener m_listener;
		unsigned int m_tcpPort, m_udpPort;
		
		sf::Packet m_udpDataPacket; //everything put into these packets are sent to all clients everytime poll() does a turn
		sf::Packet m_tcpDataPacket;
		
		std::weak_ptr<Game> m_game;
		bool running = true;
};

class RemoteClient {
	public:
		RemoteClient();
		RemoteClient(const RemoteClient &copy, int port);
		RemoteClient(int id, unsigned int udpPort, sf::IpAddress address, tcp_sock_ptr_t tcpSocket, bool ready = false);
		tcp_sock_ptr_t getOutputSocket() { return m_tcpSocket; }//impl
		bool isReady() { return ready; }
		int getId() { return m_id; }//impl
		sf::IpAddress getIpAddress() { return m_address; }
		unsigned int getUDPPort() { return m_udpPort; }
		void setUDPPort(int udpPort) { m_udpPort = udpPort; }
		void lastBombPost(float duration) { m_timeSlastBombPost = duration; }
		float getLastBombPostTime() { return m_timeSlastBombPost; }
	private:
		tcp_sock_ptr_t m_tcpSocket;
		unsigned int m_udpPort;
		int m_id;
		sf::IpAddress m_address;
		bool ready = false;
		float m_timeSlastBombPost;
};

#endif 
