#include "NetworkAddress.hpp"

using namespace Module;
using namespace std;

NetworkAddress::NetworkAddress( unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port )
{
	this->address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
	this->port = port;
}

NetworkAddress::NetworkAddress( unsigned int address, unsigned short port )
{
	this->address = address;
	this->port = port;
}
		
bool NetworkAddress::operator < ( const NetworkAddress & other ) const
{
	// note: this is so we can use address as a key in std::map
	if ( address < other.address )
		return true;
	if ( address > other.address )
		return false;
	else
		return port < other.port;
}