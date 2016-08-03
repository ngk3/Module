#include "NetworkConnection.hpp"

using namespace Module;
using namespace std;

// Constructor sets everything default and clears data
NetworkConnection::NetworkConnection( unsigned int protocolId, float timeout )
{
	this->protocolId = protocolId;
	this->timeout = timeout;
	mode = None;
	running = false;
	ClearData();
}

// Stops the connection if the connection is running
NetworkConnection::~NetworkConnection()
{
	if ( running )
		Stop();
}

//Attempts to start a connection by using a socket to connect to a port
bool NetworkConnection::Start( int port )
{
	assert( !running );
	printf( "start connection on port %d\n", port );
	if ( !socket.Open( port ) )
		return false;
	running = true;
	return true;
}
		
//Resets the connection
void NetworkConnection::Stop()
{
	assert( running );
	printf( "stop connection\n" );
	ClearData();
	socket.Close();
	running = false;
}

// Set connection to be in a server-state mode and listens 		
void NetworkConnection::Listen()
{
	printf( "server listening for connection\n" );
	ClearData();
	mode = Server;
	state = Listening;
}

// Attempts to connect to NetworkAddress		
void NetworkConnection::Connect( const NetworkAddress & address )
{
	printf( "client connecting to %d.%d.%d.%d:%d\n", 
		address.GetA(), address.GetB(), address.GetC(), address.GetD(), address.GetPort() );
	ClearData();
	mode = Client;
	state = Connecting;
	this->address = address;
}

// Update the connection after deltaTime
void NetworkConnection::Update( float deltaTime )
{
	assert( running );
	// used to tell if connection has timed out
	timeoutAccumulator += deltaTime;
	if ( timeoutAccumulator > timeout )
	{
		// Do this (state connection has failed and clear data) if timeout is true
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

// Sending a packet with size 'size\ and sending data[]'		
bool NetworkConnection::SendPacket( const unsigned char data[], int size )
{
	// After making sure that connection is running and the address is accessible...
	assert( running );
	if ( address.GetAddress() == 0 )
		return false;
	// Sends the packet over the socket using Send
	unsigned char packet[size+4];
	packet[0] = (unsigned char) ( protocolId >> 24 );
	packet[1] = (unsigned char) ( ( protocolId >> 16 ) & 0xFF );
	packet[2] = (unsigned char) ( ( protocolId >> 8 ) & 0xFF );
	packet[3] = (unsigned char) ( ( protocolId ) & 0xFF );
	memcpy( &packet[4], data, size );
	return socket.Send( address, packet, size + 4 );
}

// Receiving a packet with size 'size\'
int NetworkConnection::ReceivePacket( unsigned char data[], int size )
{
	// After making sure conneciton is running and that there is indeed data being sent...
	assert( running );
	unsigned char packet[size+4];
	NetworkAddress sender;
	int bytes_read = socket.Receive( sender, packet, size + 4 );
	if ( bytes_read == 0 )
		return 0;
	if ( bytes_read <= 4 )
		return 0;
	// Takes the packet from the socket
	if ( packet[0] != (unsigned char) ( protocolId >> 24 ) || 
		 packet[1] != (unsigned char) ( ( protocolId >> 16 ) & 0xFF ) ||
		 packet[2] != (unsigned char) ( ( protocolId >> 8 ) & 0xFF ) ||
		 packet[3] != (unsigned char) ( protocolId & 0xFF ) )
		return 0;
	// Prints if connection has been eastablished and/or resets the values needed in the NetworkConnection
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

// Resets dataa to default values/ new values
void NetworkConnection::ClearData()
{
	state = Disconnected;
	timeoutAccumulator = 0.0f;
	address = NetworkAddress();
}
