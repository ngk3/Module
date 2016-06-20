#include "NetworkInterface.hpp"
#include <iostream>

using namespace Module;
using namespace std;

NetworkInterface::NetworkInterface()
{
	cout << "Hello";
}


void NetworkInterface::openServerUDP(unsigned short port)
{
	server_port = port;
	/*if (!InitializeSocket())
	{
		printf("failed to initalize sockets\n");
		return 1;
	}*/
}

void NetworkInterface::closeServerUDP(unsigned short port)
{
	cout << "Hello";
}

int main() {
	cout << "hello\n";	
}