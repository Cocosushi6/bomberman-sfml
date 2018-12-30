#include <string>
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>

#include "server.h"
#include "../game/game.h"

using namespace std;
using namespace sf;

Server::Server(unsigned int tcpPort, unsigned int udpPort, Game* game) : m_tcpPort(tcpPort), m_udpPort(udpPort), m_game(game)
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
	if(m_sockSelector.wait(milliseconds(25))) { 
		if(m_sockSelector.isReady(m_listener)) {
			
			tcp_sock_ptr_t socket = make_unique<TcpSocket>();
			Socket::Status status = m_listener.accept(*socket);
			
			if(status == Socket::Done) {
				//Client is added here, and marked ready once it has sent back its UDP port (see parsePacket method)
				//TODO add method in game.h to create a new player -> the game should handle the spawn mechanics, etc. and return a new id
				int newID = m_game->attribID();
				m_game->addEntity(newID, move(make_unique<Player>(50, 50, 64, 64, m_game, newID)));
				
				//send back its id
				Packet clientData;
				clientData << newID << *m_game;
				socket->send(clientData);
				
				m_sockSelector.add(*socket);
				//TODO add method in server.h, class Server, to register a new client with the specified parameters
				IpAddress socketAddr = socket->getRemoteAddress();
				rclient_ptr_t newClient = make_unique<RemoteClient>(newID, -1, socketAddr, move(socket));
				addClient(newID, move(newClient));

				cout << "Added new client ! ID : " << newID << endl;
			} else {
				cout << "Error while adding new Client ! Status : " << status << endl;
			}
		}
		for(auto it = m_clients.begin(); it != m_clients.end(); ++it) {
			RemoteClient* client = it->second.get();
			TcpSocket* sock = client->getOutputSocket();
			if(m_sockSelector.isReady(*sock)) {
				Packet packet;
				Socket::Status status = sock->receive(packet);
				if(status == Socket::Done) {
					parsePacket(packet, client->getIpAddress());
				} else {
					cout << "Error while receiving TCP packet of client with id " << it->second->getId() << ", status is " << status << endl;
					if(status == Socket::Disconnected) {
						int discoID = client->getId();
						cout << "Player with id " << discoID << " was disconnected. " << endl;
						removeClient(discoID);
						m_sockSelector.remove(*sock);
						m_game->removeEntity(discoID);
						m_tcpDataPacket << "REMOVEPLAYER" << discoID;
						cout << "removed all data concerning client with id " << discoID << endl;
						//bug with iterator (which loops again, even if m_clientManager.getClients() is empty), is avoided here (was segfaulting before)
						break;
					}
				}
			}
		}
		if(m_sockSelector.isReady(m_udpSocket)) {
			IpAddress sender;
			unsigned short int port;
			Packet packet;
			Socket::Status status = m_udpSocket.receive(packet, sender, port);
			if(status != Socket::Done) {
				cout << "Error while receiving UDP packet from address " << sender << endl;
			} else {
				parsePacket(packet, sender);
			}
		}
	}
	
	//TODO change this
	//update timeSincelastBombPosed of remote clients (this is part of the really ugly things to change later)
	for(map<int, rclient_ptr_t>::iterator it = m_clients.begin(); it != m_clients.end(); it++) {
		it->second->lastBombPost(it->second->getLastBombPostTime() + delta);
	}
	
	//Don't send if nothing in packet
	if(m_udpDataPacket.getData() != NULL) {
		sendUDPToAll(m_udpDataPacket);
		m_udpDataPacket.clear();
	}
	if(m_tcpDataPacket.getData() != NULL) {
		sendTCPToAll(m_tcpDataPacket);
		m_tcpDataPacket.clear();
		cout << "Sent TCP packet to all clients" << endl;
	}
}

int Server::sendTCP(int clientID, Packet packet)
{
	cout << "Sending packet !" << endl;
	RemoteClient* client = getClient(clientID);
	if(client == nullptr) { 
		cout << "SendTCP : no such client with id " << clientID << endl;
		return -2; 
	}
	if(client->isReady()) {
		TcpSocket* out = client->getOutputSocket();
		
		Socket::Status status =  out->send(packet);
		if(status != Socket::Done) {
			string str;
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
	} else {
		cout << "Client " << clientID << " not ready ! not sending TCP packet. " << endl;
	}
	return 0;
}

int Server::sendUDP(int clientID, Packet packet)
{
	RemoteClient* client = getClient(clientID);
	if(client == nullptr) { return -2; }

	if(client->isReady()) {
		if(m_udpSocket.send(packet, client->getIpAddress(), client->getUDPPort()) != Socket::Done) {
			cout << "Error while sending UDP data to " << client->getIpAddress() << ". " << endl;
			return -1;
		}
	}
	
	return 0;
}

void Server::sendTCPToAll(Packet packet) {
	for(auto it = m_clients.begin(); it != m_clients.end(); it++) {
		cout << "sending to : " << it->first << endl;
		sendTCP(it->first, packet);
	}
}

void Server::sendUDPToAll(Packet packet)
{
	for(auto it = m_clients.begin(); it != m_clients.end(); it++) {
		sendUDP(it->first, packet);
	}
}

int Server::parsePacket(Packet packet, IpAddress sender)
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
		} catch(out_of_range e) {
			cout << "no such client" << endl;
			continue;
		}
	
		if(descriptor == "UDPPORT") {
			int udpPort;
			if(!(packet >> udpPort)) continue;

			RemoteClient* client = getClient(clientID);
			if(client == nullptr) continue;
			client->setUDPPort(udpPort);
			client->setReady();

			cout << "Client sent UDP port" << endl;

		} else if(descriptor == "ADDBOMB") {
			cout << descriptor << endl;

			RemoteClient* client = getClient(clientID);
			if(client == nullptr) {
				cout << "Client is nullptr ! not adding bomb" << endl;
				continue; 
			}
			if(client->getLastBombPostTime() >= 0.3f) {
				auto player = m_game->getEntity(clientID);
				if(player != nullptr) {
					// the + SPRITE_SIZE / 2 is to have the position under the player's feet
					Vector2<int> bombCoords = toTileCoordinates((int)(player->getPosition().x + player->getSize().x / 2), (int)(player->getPosition().y + player->getSize().y / 2 + 5)) * TILE_SIZE; 
					
					unique_ptr<Bomb> b = make_unique<Bomb>(bombCoords.x, bombCoords.y, BOMB_DURATION, m_game, m_game->attribID());
					int id = b->getID();
					m_game->addBomb(id, move(b));
					client->lastBombPost(0.0f);
				} else {
					cout << descriptor << " : no such player with id : " << clientID << endl;
				}
			} else {
				cout << "Client " << clientID << " tries to add too much bombs ! (delta : " << m_clients.at(clientID)->getLastBombPostTime() << ")"  << endl;
			}
		} else if(descriptor == "INPUTSTATE") {
			auto p = m_game->getEntity(clientID);
			if(p != nullptr) {
				InputState state;
				if(!(packet >> state)) { cout << "skip" << endl; continue; }
				bool currentlyMoving = true; //TODO break here check state
				if(state.state) {
					switch(state.key) {
						case Keyboard::S :
							p->move(0.0f, 1.0f, state.delta);
							p->setDirection(DOWN);
							break;
						case Keyboard::Z : 
							p->move(0.0f, -1.0f, state.delta);
							p->setDirection(UP);
							break;
						case Keyboard::Q :
							p->move(-1.0f, 0.0f, state.delta);
							p->setDirection(LEFT);
							break;
						case Keyboard::D : 
							p->move(1.0f, 0.0f, state.delta);
							p->setDirection(RIGHT);
							break;
						default:
							currentlyMoving = false;
					}
				} else {
					currentlyMoving = false;
				}
				p->setMoving(currentlyMoving);
				p->update();
				m_udpDataPacket << "PLAYERMOVE" << clientID << state.timestamp << state.delta << p->getPosition().x << p->getPosition().y << p->getDirection() << p->isMoving(); 
// 				cout << "player " << clientID << " moved @ coordinates " << p->getX() << ", " << p->getY() << endl;
			}
		}
	}
	
	return 0;
}

void Server::onNotify(int objectID, Subject *sub, ::Event ev, Uint64 timestamp)
{
	if(ev == EVENT_BOMB_ADD) {
		Bomb *bomb = m_game->getBomb(objectID);
		cout << bomb->toString() << endl;
		m_tcpDataPacket << "ADDBOMB" << *(m_game->getBomb(objectID));
	} else if(ev == EVENT_BOMB_EXPLODED) {
		m_tcpDataPacket << "BOMBEXPLODE" << objectID;
	} else if(ev == EVENT_BOMB_DIED) {
		m_tcpDataPacket << "REMOVEBOMB" << objectID;
	} else if(ev == EVENT_PLAYER_JOIN) {
		m_tcpDataPacket << "ADDPLAYER" << *(dynamic_cast<Player*>(m_game->getEntity(objectID)));
	}
	//TODO check all events fired in game and implement something for them here (eg : ADDPLAYER)
}

void Server::addClient(int id, rclient_ptr_t client)
{
	m_clients.insert(pair<int, rclient_ptr_t>(id, move(client)));
}

void Server::removeClient(int id)
{
	m_clients.erase(id);
}

RemoteClient* Server::getClient(int id)
{
	try {
		return m_clients.at(id).get();
	} catch(out_of_range ex) {
		cout << "No such remote client for id " << id << endl;
		return nullptr;
	}
}

RemoteClient::RemoteClient()
{
	
}

RemoteClient::RemoteClient(int id, unsigned int udpPort, IpAddress address, tcp_sock_ptr_t tcpSocket, bool ready) : m_id(id), m_address(address), m_udpPort(udpPort), m_tcpSocket(move(tcpSocket)), ready(ready)
{
	
}



