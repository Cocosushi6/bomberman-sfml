#include "client.h"
#include "../game/input.h"
#include "../game/bomb.h"
#include "../utils/utils.h"
#include "../utils/player_manag.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <memory>
#include <tuple>
#include <string>

using namespace std;
using namespace sf;

Client::Client(sf::IpAddress address, int tcpServerPort, int udpServerPort, int udpPort) :  m_tcpServerPort(tcpServerPort), m_udpServerPort(udpServerPort), m_udpPort(udpPort), m_serverAddr(address),  m_tcpDataPacket(), m_udpDataPacket()
{
}

Client::~Client()
{
	//TODO add call to disconnect() for sockets 
}

tuple<unique_ptr<Game>, int> Client::connect()
{
	//server connection
	Socket::Status status = m_tcpSock.connect(m_serverAddr, m_tcpServerPort);
	if(status != Socket::Done) {
		cout << "Failed to connect tcp socket to server ! Aborting." << endl;
		return {nullptr, -1};
	}

	status = m_udpSock.bind((m_udpPort == -1) ? Socket::AnyPort : m_udpPort);

	if(status != Socket::Done) {
		cout << "Failed to connect udp socket to server ! Aborting." << endl;
		return {nullptr, -1};
	}

	if(m_udpPort == -1) m_udpPort = m_udpSock.getLocalPort();
	m_connected = true;

	///receive data from server
	sf::Packet gamePacket;
	status = m_tcpSock.receive(gamePacket);
	if(status != Socket::Done) {
		cout << "Couldn't receive game data from server, aborting." << endl;
		return {nullptr, -1};
	}
	
	//transform data
	//Get ID
	int id;
	if(!(gamePacket >> id)) {
		cout << "No id in packet. Aborting. " << endl;
		return {nullptr, -1};
	}

	if(id >= 0) {
		setID(id); //TODO setID should be used only once : here.
	}
	
	//Get game data
	unique_ptr<Game> game = make_unique<Game>();
	if(!(gamePacket >> *game)) {
		cout << "Couldn't extract game data. Aborting." << endl;
		return {nullptr, -1};
	}
	
	m_game = game.get();

	//Send back udp port
	sf::Packet udpPort;
	string descriptor = "UDPPORT";
	udpPort << descriptor << id << m_udpPort;
	m_tcpSock.send(udpPort);
	
	m_tcpSock.setBlocking(false);
	m_udpSock.setBlocking(false);
	cout << "Connected to server" << endl;

	return {move(game), id};
}

void Client::poll()
{
	//UDP receive
	IpAddress sender;
	unsigned short senderPort;
	
	sf::Packet dataPacket;
	
	Socket::Status status = m_udpSock.receive(dataPacket, sender, senderPort);
	while(status != Socket::NotReady) {
		if(status != Socket::Done && status != Socket::NotReady) {
			cout << "Error while receiving UDP packet, status : " << status << endl;
		} else if(status == Socket::Done) {
			// 		cout << "client.cpp : udp packet received" << endl;
			parsePacket(dataPacket);
		}
		status = m_udpSock.receive(dataPacket, sender, senderPort);
	}
	
	//TCP receive
	status = m_tcpSock.receive(dataPacket);
	if(status != Socket::Done && status != Socket::NotReady) {
		cout << "Error while reading TCP packet, status : " << status << endl;
		if(status == Socket::Disconnected) {
			cout << "Disconnected from server, aborting. (status : 3)" << endl;
			m_connected = false;
		}
	} else if(status == Socket::Done) {
 		cout << "Client.cpp : tcp packet received" << endl;
		parsePacket(dataPacket);
	} 
	
	//Send data queue
	if(!(m_udpDataPacket.getData() == NULL)) {
		sendUDP(m_udpDataPacket);
		m_udpDataPacket.clear();
	}
	if(!(m_tcpDataPacket.getData() == NULL)) {
		sendTCP(m_tcpDataPacket);
		m_tcpDataPacket.clear();
	}
}

int Client::parsePacket(sf::Packet packet)
{
	while (!packet.endOfPacket())
	{

		string descriptor;
		if (!(packet >> descriptor))
		{
			cout << "No descriptor, failed to parse packet." << endl;
			continue;
		}


		if (descriptor == "ADDBOMB")
		{
			bomb_ptr_t b = make_unique<Bomb>(m_game);
			if (!(packet >> *b)) {
				cout << "Couldn't unpack bomb !" << endl;
				continue;
			}			
			cout << descriptor << b->toString() << endl;
			int id = b->getID();
			m_game->addBomb(id, move(b));
		}
		else if (descriptor == "BOMBEXPLODE")
		{
			int id;
			if (!(packet >> id))
				continue;

			Bomb *bomb = m_game->getBomb(id);
			if (bomb != nullptr)
			{
				bomb->explode();
			}
		}
		else if (descriptor == "REMOVEBOMB")
		{
			int id;
			if (!(packet >> id))
				continue;

			Bomb *bomb = m_game->getBomb(id);
			if (bomb != nullptr)
			{
				bomb->die();
			}
		}
		else if (descriptor == "ADDPLAYER")
		{
			entity_ptr_t p = make_unique<Player>();
			if (!(packet >> *p))
				continue;

			int id = p->getID();
			m_game->addEntity(id, move(p)); //eval order is unspecified, so if move(p) is called before p->getID(), then ... SEGFAULT !
		}
		else if (descriptor == "REMOVEPLAYER")
		{
			int id;
			if (!(packet >> id))
				continue;
			m_game->removeEntity(id);
		}
		else if (descriptor == "PLAYERMOVE")
		{
			int entID;
			if (!(packet >> entID))
			{
				cout << "skip" << endl;
				continue;
			}
			Player* curPlayer = m_game->getEntity(entID);
			if (curPlayer != nullptr)
			{
				sf::Uint64 timestamp;
				//newX, newY and direction are the values at timestamp, server-side
				float newX, newY, delta;
				string direction;
				bool moving;

				if (!(packet >> timestamp >> delta >> newX >> newY >> direction >> moving))
				{
					cout << "skip4" << endl;
					continue;
				}
				if (entID == m_id)
				{
					PlayerManager* manager = curPlayer->getManager();
					if (manager != nullptr)
					{ 
						try
						{
							Player oldState = manager->getPStateMgr().get((sf::Uint64)timestamp);

							//Check validity
							if (oldState.getNewX() != newX)
							{
								float oldX = oldState.getX();
								curPlayer->setX(newX);
								curPlayer->setNewX(newX);
								cout << timestamp;
								cout << " correcting x, old : " << oldX << ", new : " << newX << ", diff : " << newX - oldX << endl;
							}
							if (oldState.getNewY() != newY)
							{
								float oldY = oldState.getY();
								curPlayer->setY(newY);
								curPlayer->setNewY(newY);
								cout << timestamp;
								cout << " correcting y, old : " << oldY << ", new : " << newY << ", diff : " << newY - oldY << endl;
							}
							if (oldState.getDirection() != direction)
							{
								curPlayer->setDirection(direction);
							}
							if (oldState.isMoving() != moving)
							{
								curPlayer->setMoving(moving);
							}
						}
						catch (out_of_range e)
						{
							cout << "no such state with timestamp " << timestamp << endl;
						}
					}
					else
					{
						cout << "Player does not have a valid PlayerManager" << endl;
						continue;
					}
				}
				else
				{
					cout << getTimestamp() << " moved other player, x : " << newX << ", y : " << newY << endl;
					curPlayer->setLastDelta(delta);
					curPlayer->setNewX(newX);
					curPlayer->setNewY(newY);
					curPlayer->setDirection(direction);
					curPlayer->setMoving(moving);
					curPlayer->udpate();
				}
			}
			else
			{
				cout << "No such player in game !" << endl;
				continue;
			}
		}
	}

	return 0;
}

int Client::sendTCP(sf::Packet packet)
{
	sf::Socket::Status status = m_tcpSock.send(packet);
	while(status != sf::Socket::Done) {
		status = m_tcpSock.send(packet);
		if(status != sf::Socket::Done || status != sf::Socket::Partial) {
			cout << "Error while sending tcp socket : status " << status << endl;
			return -1;
		}
	}
	return 0;
}

int Client::sendUDP(sf::Packet packet)
{
	sf::Socket::Status status = m_udpSock.send(packet, m_serverAddr, m_udpServerPort);
	while(status != sf::Socket::Done) {
		status = m_udpSock.send(packet, m_serverAddr, m_udpServerPort);
		if(status != sf::Socket::Done || status != sf::Socket::Partial) {
			cout << "Error while sending udp socket : status " << status << endl;
			return -1;
		}
	}
	return 0;
}


int Client::getID()
{
	return m_id;
}

void Client::onNotify(int objectID, Subject *sub, ::Event ev, sf::Uint64 timestamp)
{
}
