#include <iostream>
#include <memory>
#include "game/client_launcher.h"
#include "game/server_launcher.h"
#include "utils/utils.h"
#include <functional>
#include <SFML/Network.hpp>

using namespace std;
using namespace sf;

int launchClient();
int launchServer();

int main() {
	function<bool(int)> checkChoice = [](int choice) {
		return (choice == 1 || choice == 2);
	};

	int choice = 0;
	prompt("Choose 1 for Client or 2 for Server : ", choice, checkChoice);

	if(choice == 1) return launchClient();
	if(choice == 2) return launchServer();
	return -1;
}

int launchClient() {
	ClientGame game{};
	unsigned int udpPort = 0, tcpPort = 0, localUdpPort = 0;
	std::string nickname;
	std::string ipAddress;

	function<bool(unsigned int)> checkPort = [](unsigned int port) {
		return port >= 1 && port <= 65535;
	};

	function<bool(std::string)> checkNickname = [](std::string nickname) {
		return nickname.length() > 0 && nickname.length() < 16;
	};

	function<bool(std::string)> checkIpAddress = [](std::string ipAddres) {
		return true;
	};

	prompt("Please enter the server's IP Address : ", ipAddress, checkIpAddress);
	prompt("Please enter the server's UDP port : ", udpPort, checkPort);
	prompt("Please enter the server's TCP port : ", tcpPort, checkPort);
	prompt("Please enter the local UDP port to bind the client to :", localUdpPort, checkPort);

	prompt("Please enter nickname : ", nickname, checkNickname);

	return game.run(ipAddress, tcpPort, udpPort, localUdpPort, nickname);
}

int launchServer() {
	ServerGame server{};
	unsigned int udpPort = 0, tcpPort = 0;

	function<bool(unsigned int)> checkPort = [](unsigned int port){ 
		return port >= 1 && port <= 65535;
	};

	prompt("Please enter UDP Port : ", udpPort, checkPort);
	prompt("Please enter TCP Port : ", tcpPort, checkPort);

	return server.run(tcpPort, udpPort);
}