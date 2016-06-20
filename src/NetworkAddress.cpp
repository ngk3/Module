#include "NetworkAddress.hpp"

using namespace Module;

NetworkAddress::NetworkAddress() : address(0), port(0) {}
	
NetworkAddress::NetworkAddress( unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port )
{
	this->address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
	this->port = port;
}
	
NetworkAddress::NetworkAddress( unsigned int Networkaddress, unsigned short port )
{
	this->address = address;
	this->port = port;
}
