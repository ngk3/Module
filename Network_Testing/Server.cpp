/*
	Server using Virtual Connection over UDP
	From "Networking for Game Programmers" - http://www.gaffer.org/networking-for-game-programmers
	Author: Glenn Fiedler <gaffer@gaffer.org>
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

#include "NetworkInterface.hpp"

using namespace std;
using namespace Module;

const int ServerPort = 30000;
const int ClientPort = 30001;
const int ProtocolId = 0x99887766;
const float DeltaTime = 0.25f;
const float SendRate = 0.25f;
const float TimeOut = 10.0f;

int main( int argc, char * argv[] )
{
	if ( !InitializeSockets() )
	{
		printf( "failed to initialize sockets\n" );
		return 1;
	}

	NetworkConnection connection( ProtocolId, TimeOut );
	
	if ( !connection.Start( ServerPort ) )
	{
		printf( "could not start connection on port %d\n", ServerPort );
		return 1;
	}
	
	connection.Listen();
	
	while ( true )
	{
		if ( connection.IsConnected() )
		{
			unsigned char packet[256];
			string Input;
			getline(cin, Input);
			memcpy(packet, Input.c_str(), sizeof(Input));
			//unsigned char packet[] = "server to client";
			connection.SendPacket( packet, sizeof( packet ) );
		}
		
		while ( true )
		{
			unsigned char packet[256];
			int bytes_read = connection.ReceivePacket( packet, sizeof(packet) );
			if (bytes_read != 0)
				cout << "Client Message:" << packet << endl;
			if ( bytes_read == 0 )
				break;
		}
		
		connection.Update( DeltaTime );
		wait( DeltaTime );
	}
	
	ShutdownSockets();

	return 0;
}
