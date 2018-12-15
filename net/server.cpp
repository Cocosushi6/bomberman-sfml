#include <string>
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>

#include "server.h"
#include "../game/game.h"

using namespace std;
using namespace sf;

Server::Server(unsigned int tcpPort, unsigned int udpPort, std::weak_ptr<Game> game) : m_tcpPort(tcpPort), m_udpPort(udpPort), m_game(game)
{}

int Server::init()
{
	if(m_listener.listen(m_tcpPort) != Socket::Done) {
		cout << "Failed to listen on port " << m_tcpPort << " ! Aborting. " << endl;
		return -1;
	}
	if(m_udpSocket.bind(m_udpPort) != Socket::Done) {
		cout << "Failed to bind udp port " << m_udpPort << " ! Aborting. " << endl;
		return -2;
	}
	m_sockSelector.add(m_listener);
	m_sockSelector.add(m_udpSocket);
	return 0;
}


void Server::poll(float delta)
{
	if(m_sockSelector.wait(sf::milliseconds(25))) { 
		if(m_sockSelector.isReady(m_listener)) {
			
			tcp_sock_ptr_t socket(new TcpSocket()); //TODO replace with make_shared
			Socket::Status status = m_listener.accept(*socket);
			
			if(status == Socket::Done) {
				//Client is added here, and marked ready once it has sent back its UDP port (see parsePacket method)
				int newID = m_game.lock()->attribID();
				m_game.lock()->addEntity(newID, std::make_shared<Player>(50, 50, m_game, newID));
				rclient_ptr_t newClient = std::make_shared<RemoteClient>(newID, -1, socket->getRemoteAddress(), socket);
				addClient(newID, newClient);
				
				//send back its id
				Packet clientData;
				clientData << newID << *m_game.lock(); //warning : might bug !
				socket->send(clientData);
				
				m_sockSelector.add(*socket);
				cout << "Added new client ! ID : " << newID << endl;
			} else {
				cout << "Error while adding new Client ! Status : " << status << endl;
			}
		}
		for(std::map<int, rclient_ptr_t>::iterator it = m_clients.begin(); it != m_clients.end(); ++it) {
			rclient_ptr_t client = it->second;
			tcp_sock_ptr_t sock = client->getOutputSocket();
			if(m_sockSelector.isReady(*sock)) {
				sf::Packet packet;
				Socket::Status status = sock->receive(packet);
				if(status == sf::Socket::Done) {
					parsePacket(packet, client->getIpAddress());
				} else {
					cout << "Error while receiving packet of client with id " << it->second->getId() << ", status is " << status << endl;
					if(status == Socket::Disconnected) {
						int discoID = client->getId();
						cout << "Player with id " << discoID << " was disconnected. " << endl;
						removeClient(discoID);
						m_sockSelector.remove(*sock);
						m_game.lock()->removeEntity(discoID);
						m_tcpDataPacket << "REMOVEPLAYER" << discoID;
						cout << "removed all data concerning client with id " << discoID << endl;
						//bug with iterator (which loops again, even if m_clientManager.getClients() is empty), is avoided here (was segfaulting before)
						break;
					}
				}
			}
		}
		if(m_sockSelector.isReady(m_udpSocket)) {
			sf::IpAddress sender;
			unsigned short int port;
			sf::Packet packet;
			sf::Socket::Status status = m_udpSocket.receive(packet, sender, port);
			if(status != Socket::Done) {
				cout << "Error while receiving UDP packet from address " << sender << endl;
			} else {
				parsePacket(packet, sender);
			}
		}
	}
	
	//TODO change this
	//update timeSincelastBombPosed of remote clients (this is part of the really ugly things to change later)
	for(std::map<int, rclient_ptr_t>::iterator it = m_clients.begin(); it != m_clients.end(); it++) {
		it->second->lastBombPost(it->second->getLastBombPostTime() + delta);
	}
	
	//Don't send if nothing in packet
	if(m_udpDataPacket.getData() != NULL) {
		sendUDPToAll(m_udpDataPacket);
		m_udpDataPacket.clear();
		cout << "sent udp packet" << endl;
	}
	if(m_tcpDataPacket.getData() != NULL) {
		sendTCPToAll(m_tcpDataPacket);
		m_tcpDataPacket.clear();
		cout << "sent tcp packet "<< endl;
	}
}

int Server::sendTCP(int clientID, sf::Packet packet)
{
	if(m_clients.at(clientID)->isReady()) {
		tcp_sock_ptr_t out = m_clients.at(clientID)->getOutputSocket();
		
		sf::Socket::Status status =  out->send(packet);
		if(status != Socket::Done) {
			std::string str;
			if(status == Socket::Error) {
				str= "error";
			} else if(status == Socket::Disconnected) {
				str = "disconnected";
				removeClient(clientID);
			} else if(status == Socket::NotReady) {
				str = "not ready";
			} else if(status == Socket::Partial) {
				str = "partial";
			}
			cout << "Error while sending TCP packet to client with id " << clientID << ". Socket status : " <<  str << endl;
			return -1;
		}
		return 0;
	}
}

int Server::sendUDP(int clientID, sf::Packet packet)
{
	if(m_clients.at(clientID)->isReady()) {
		IpAddress recipient = m_clients.at(clientID)->getIpAddress();
		int clientPort = m_clients.at(clientID)->getUDPPort();
		
		if(m_udpSocket.send(packet, recipient, clientPort) != Socket::Done) {
			cout << "Error while sending UDP data to " << recipient << ". " << endl;
			return -1;
		}
	}
	
	return 0;
}

void Server::sendTCPToAll(sf::Packet packet) {
	for(std::map<int, rclient_ptr_t>::iterator it = m_clients.begin(); it != m_clients.end(); it++) {
		sendTCP(it->first, packet);
	}
}

void Server::sendUDPToAll(sf::Packet packet)
{
	for(std::map<int, rclient_ptr_t>::iterator it = m_clients.begin(); it != m_clients.end(); it++) {
		sendUDP(it->first, packet);
	}
}

int Server::parsePacket(sf::Packet packet, sf::IpAddress sender)
{
	while(!packet.endOfPacket()) {
		string descriptor;
		if(!(packet >> descriptor)) {
			cout << "Failed to get descriptor : skipping packet" << endl;
			continue;
		} else {
// 			cout << descriptor << endl;
		}
	
		int clientID;
		if(!(packet >> clientID)) {
			cout << "Failed to get client id : skipping packet" << endl;
			continue;
		}
		
		try {
			if(sender != m_clients.at(clientID)->getIpAddress()) {
				cout << "Sender's IP address does not match the ID he has sent. To prevent spoofing, the packet will be discarded" << endl;
				continue;
			}
		} catch(std::out_of_range e) {
			cout << "no such client" << endl;
			continue;
		}
	
		if(descriptor == "UDPPORT") {
			int udpPort;
			if(!(packet >> udpPort)) continue;
			rclient_ptr_t client = m_clients.at(clientID);
			client = std::make_shared<RemoteClient>(*client, udpPort); //warning, might bug
			m_clients[clientID] = client;
			cout << "Client sent UDP port" << endl;
		} else if(descriptor == "ADDBOMB") {
			if(auto game = m_game.lock()) {
				cout << game->toString() << endl;
				if(m_clients.at(clientID)->getLastBombPostTime() >= 0.3f) {
					if(auto player = game->getEntity(clientID).lock()) {
						// the + SPRITE_SIZE / 2 is to have the position under the player's feet
						sf::Vector2<int> bombCoords = toTileCoordinates((int)(player->getX() + SPRITE_SIZE / 2), (int)(player->getY() + SPRITE_SIZE / 2 + 5)) * TILE_SIZE; 
						
						std::shared_ptr<Bomb> b = std::make_shared<Bomb>(bombCoords.x, bombCoords.y, BOMB_DURATION, m_game, game->attribID());
						game->addBomb(b->getID(), b);
						m_clients.at(clientID)->lastBombPost(0.0f);
					}
				} else {
					cout << "Client " << clientID << " tries to add too much bombs ! (delta : " << m_clients.at(clientID)->getLastBombPostTime() << ")"  << endl;
				}
			} else {
				cout << "Couldn't lock game ptr" << endl;
			}
		} else if(descriptor == "INPUTSTATE") {
			if(auto p = m_game.lock()->getEntity(clientID).lock()) {
				InputState state;
				if(!(packet >> state)) { cout << "skip" << endl; continue; }
				bool currentlyMoving = true; //TODO break here check state
				if(state.state) {
					switch(state.key) {
						case sf::Keyboard::S :
							p->move(0.0f, 1.0f, state.delta);
							p->setDirection("down");
							break;
						case sf::Keyboard::Z : 
							p->move(0.0f, -1.0f, state.delta);
							p->setDirection("up");
							break;
						case sf::Keyboard::Q :
							p->move(-1.0f, 0.0f, state.delta);
							p->setDirection("left");
							break;
						case sf::Keyboard::D : 
							p->move(1.0f, 0.0f, state.delta);
							p->setDirection("right");
							break;
						default:
							currentlyMoving = false;
					}
				} else {
					currentlyMoving = false;
				}
				p->setMoving(currentlyMoving);
				p->udpate();
				m_udpDataPacket << "PLAYERMOVE" << clientID << state.timestamp << state.delta << p->getX() << p->getY() << p->getDirection() << p->isMoving(); 
// 				cout << "player " << clientID << " moved @ coordinates " << p->getX() << ", " << p->getY() << endl;
			}
		}
	}
	
	return 0;
}

void Server::onNotify(int objectID, Subject *sub, ::Event ev, sf::Uint64 timestamp)
{
	if(ev == EVENT_BOMB_ADD) {
		if(auto game = m_game.lock()) {
			m_tcpDataPacket << "ADDBOMB" << *game->getBomb(objectID).lock();
		}
	} else if(ev == EVENT_BOMB_EXPLODED) {
		m_tcpDataPacket << "BOMBEXPLODE" << objectID;
	} else if(ev == EVENT_BOMB_DIED) {
		m_tcpDataPacket << "REMOVEBOMB" << objectID;
	} else if(ev == EVENT_PLAYER_JOIN) {
		if(auto game = m_game.lock()) {
			m_tcpDataPacket << "ADDPLAYER" << *game->getEntity(objectID).lock();
		}
	}
	//TODO check all events fired in game and implement something for them here (eg : ADDPLAYER)
}

void Server::addClient(int id, rclient_ptr_t client)
{
	m_clients.insert(std::pair<int, rclient_ptr_t>(id, client));
}

void Server::removeClient(int id)
{
	m_clients.erase(id);
}

std::weak_ptr<RemoteClient> Server::getClient(int id)
{
	return m_clients.at(id);
}

RemoteClient::RemoteClient()
{
	
}

RemoteClient::RemoteClient(const RemoteClient& copy, int port) : m_id(copy.m_id), m_address(copy.m_address), m_udpPort(port), m_tcpSocket(copy.m_tcpSocket), ready(true)
{
	
}

RemoteClient::RemoteClient(int id, unsigned int udpPort, sf::IpAddress address, tcp_sock_ptr_t tcpSocket, bool ready) : m_id(id), m_address(address), m_udpPort(udpPort), m_tcpSocket(tcpSocket), ready(ready)
{
	
}



