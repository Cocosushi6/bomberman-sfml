#ifndef SRC_CLIENT_H
#define SRC_CLIENT_H

#include "../game/game.h"
#include <SFML/Network.hpp>
#include <memory>
#include <tuple>
#include <string>
#include <iostream>
#include "../utils/subject.h"
#include "../utils/observer.h"

class Client : public Subject, public Observer {
	public:
		Client(sf::IpAddress address, int serverPort);
		~Client();
		std::tuple<std::shared_ptr<Game>, int> connect();
		void poll();
		int getID();
		
		template<typename... Args>
		void sendTCPMessage(std::string descriptor, Args... args) {
			m_tcpDataPacket << descriptor << m_id;
			addToPacket(m_tcpDataPacket, args...);
		}
		
		template<typename... Args> 
		void sendUDPMessage(std::string descriptor, Args... args) {
			m_udpDataPacket << descriptor << m_id;
			addToPacket(m_udpDataPacket, args...);
		}
		
		virtual void onNotify(int objectID, Subject *sub, Event ev, sf::Uint64 timestamp);
	private:
		//in case args is completely empty for the beginng (see sendUDPMessage,sendTCPMessage)
		void addToPacket(sf::Packet& packet) {
		}
		
		template<typename T>
		void addToPacket(sf::Packet& packet, T value) {
			packet << value;
		}
		
		template<typename T, typename... Args> 
		void addToPacket(sf::Packet &packet, T value, Args... args) {
			packet << value;
			addToPacket(packet, args...);
		}
		
		int sendUDP(sf::Packet packet);
		int sendTCP(sf::Packet packet);
		void setID(int id) { m_id = id; }
		int parsePacket(sf::Packet packet);

		int m_id = -1;
		int m_udpPort = -1;
		int m_serverPort;
		bool m_connected = false;
		
		std::weak_ptr<Game> m_game;

		sf::UdpSocket m_udpSock;
		sf::TcpSocket m_tcpSock;
		sf::IpAddress m_serverAddr;
		sf::Packet m_tcpDataPacket;
		sf::Packet m_udpDataPacket;
};

#endif /* SRC_CLIENT_H */
