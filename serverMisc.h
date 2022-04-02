#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <thread>
#include <mutex>

#include "quickWrite.h"
#include "client.h"
#include "serverMisc.h"
#include "map.h"

struct clientStruct {
	std::thread thread;
	sf::TcpSocket socket;
	bool isUsed = false;
	bool wasUsed = false;
};

sf::IpAddress getLocalIP() {
	return sf::IpAddress::getLocalAddress();
}

sf::IpAddress getPublicIP() {
	return sf::IpAddress::getLocalAddress();
}