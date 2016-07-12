#include "NetworkConnection.hpp"

using namespace Module;
using namespace std;

NetworkConnection::NetworkConnection( unsigned int protocolId, float timeout )
{
	this->protocolId = protocolId;
	this->timeout = timeout;
	mode = None;
	running = false;
	ClearData();
}

NetworkConnection::~NetworkConnection()
{
	if ( running )
		Stop();
}

bool NetworkConnection::Start( int port )
{
	assert( !running );
	printf( "start connection on port %d\n", port );
	if ( !socket.Open( port ) )
		return false;
	running = true;
	return true;
}
		
void NetworkConnection::Stop()
{
	assert( running );
	printf( "stop connection\n" );
	ClearData();
	socket.Close();
	running = false;
}
		
void NetworkConnection::Listen()
{
	printf( "server listening for connection\n" );
	ClearData();
	mode = Server;
	state = Listening;
}
		
void NetworkConnection::Connect( const NetworkAddress & address )
{
	printf( "client connecting to %d.%d.%d.%d:%d\n", 
		address.GetA(), address.GetB(), address.GetC(), address.GetD(), address.GetPort() );
	ClearData();
	mode = Client;
	state = Connecting;
	this->address = address;
}

void NetworkConnection::Update( float deltaTime )
{
	assert( running );
	timeoutAccumulator += deltaTime;
	if ( timeoutAccumulator > timeout )
	{
		if ( state == Connecting )
		{
			printf( "connect timed out\n" );
			ClearData();
			state = ConnectFail;
		}
		else if ( state == Connected )
		{
			printf( "connection timed out\n" );
			ClearData();
			if ( state == Connecting )
				state = ConnectFail;
		}
	}
}
		
bool NetworkConnection::SendPacket( const unsigned char data[], int size )
{
	assert( running );
	if ( address.GetAddress() == 0 )
		return false;
	unsigned char packet[size+4];
	packet[0] = (unsigned char) ( protocolId >> 24 );
	packet[1] = (unsigned char) ( ( protocolId >> 16 ) & 0xFF );
	packet[2] = (unsigned char) ( ( protocolId >> 8 ) & 0xFF );
	packet[3] = (unsigned char) ( ( protocolId ) & 0xFF );
	memcpy( &packet[4], data, size );
	return socket.Send( address, packet, size + 4 );
}

int NetworkConnection::ReceivePacket( unsigned char data[], int size )
{
	assert( running );
	unsigned char packet[size+4];
	NetworkAddress sender;
	int bytes_read = socket.Receive( sender, packet, size + 4 );
	if ( bytes_read == 0 )
		return 0;
	if ( bytes_read <= 4 )
		return 0;
	if ( packet[0] != (unsigned char) ( protocolId >> 24 ) || 
		 packet[1] != (unsigned char) ( ( protocolId >> 16 ) & 0xFF ) ||
		 packet[2] != (unsigned char) ( ( protocolId >> 8 ) & 0xFF ) ||
		 packet[3] != (unsigned char) ( protocolId & 0xFF ) )
		return 0;
	if ( mode == Server && !IsConnected() )
	{
		printf( "server accepts connection from client %d.%d.%d.%d:%d\n", 
			sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(), sender.GetPort() );
		state = Connected;
		address = sender;
	}
	if ( sender == address )
	{
		if ( mode == Client && state == Connecting )
		{
			printf( "client completes connection with server\n" );
			state = Connected;
		}
		timeoutAccumulator = 0.0f;
		memcpy( data, &packet[4], size);
		return size;
	}
	return 0;
}

void NetworkConnection::ClearData()
{
	state = Disconnected;
	timeoutAccumulator = 0.0f;
	address = NetworkAddress();
}
