#include "NetworkSocket.hpp"
#include <iostream>
#include <assert.h>
#include <unistd.h>


// platform detection

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS

	#include <winsock2.h>
	#pragma comment( lib, "wsock32.lib" )

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <fcntl.h>

#else

	#error unknown platform!

#endif


using namespace Module;
using namespace std;

NetworkSocket::NetworkSocket() : socket(0) {}

NetworkSocket::~NetworkSocket()
{
	Close();
}

bool NetworkSocket::Open(unsigned short port) 
{
	assert(!isOpen());
	
	socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if (socket <= 0)
	{
		cout << "Failed to create socket\n";
		socket = 0;
		return false;
	}
	
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons((unsigned short) port);
	
	if(bind(socket, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0) 
	{
		cout << "Failed to bind socket\n";
		Close();
		return false;
	}
	
	#if PLATFORM != PLATFORM_WINDOWS
		
		int nonBlocking = 1;
		if ( fcntl( socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
		{
			cout << "failed to set non-blocking socket\n";
			Close();
			return false;
		}
			
		#elif PLATFORM == PLATFORM_WINDOWS
		
			DWORD nonBlocking = 1;
			if ( ioctlsocket( socket, FIONBIO, &nonBlocking ) != 0 )
			{
				cout << "failed to set non-blocking socket\n";
				Close();
				return false;
			}

	#endif
	
	return true;
	
}

void NetworkSocket::Close() 
{
	if (socket != 0)
	{
		#if PLATFORM != PLATFORM_WINDOWS
		close(socket);
		#elif PLATOFMR == PLATFORM_WINDOWS
		closesocket(socket);
		#endif
		socket = 0;
	}
}

bool NetworkSocket::isOpen() const 
{
	return socket != 0;
}

bool NetworkSocket::send(const NetworkAddress& destination, const void* data, int size)
{
	assert(data);
	assert(size > 0);
	
	if (socket == 0)
		return false;
	
	assert(destination.GetAddress() != 0);
	assert(destination.GetPort() != 0);
	
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(destination.GetAddress());
	address.sin_port = htons((unsigned short) destination.GetPort());
	
	int sent_bytes = sendto(socket, (const char*)data, size, 0, (sockaddr*)&address, sizeof(sockaddr_in));
	
	return sent_bytes == size;
}

int NetworkSocket::receive(NetworkAddress& sender, void* data, int size)
{
	assert( data );
	assert( size > 0 );
		
	if ( socket == 0 )
		return false;
			
	#if PLATFORM == PLATFORM_WINDOWS
		typedef int socklen_t;
	#endif
			
	sockaddr_in from;
	socklen_t fromLength = sizeof( from );

	int received_bytes = recvfrom( socket, (char*)data, size, 0, (sockaddr*)&from, &fromLength );

	if ( received_bytes <= 0 )
		return 0;

	unsigned int address = ntohl( from.sin_addr.s_addr );
	unsigned short port = ntohs( from.sin_port );

	sender = NetworkAddress( address, port );

	return received_bytes;
}


int main() {
	std::cout << "Hello\n";	
}