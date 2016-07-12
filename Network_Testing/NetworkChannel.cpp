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

const int PROTOCOL_ID = 0x99887766;
const float DELTA = 0.25f;
const float RATE = 0.25f;
const float TIMEOUT = 10.0f;
const unsigned char FRAGMENT = 0x80;
const unsigned char SUMMARY = 0x40;

void print(unsigned char *container, size_t size){
	for (size_t i = 0; i < size; i++){
		cout << (int)(container[i]) << "|";
	}
	cout << endl;
}

int net_inchan(int port, int ProtocolId, float DeltaTime, float TimeOut, size_t packet_size){
	if ( !InitializeSockets() )
	{
		printf( "failed to initialize sockets\n" );
		return 1;
	}

	NetworkConnection connection( ProtocolId, TimeOut );
	
	if ( !connection.Start( port ) )
	{
		printf( "could not start connection on port %d\n", port );
		return 1;
	}
	connection.Listen();

	packet_size = packet_size + 1; // every packet has 1 more character for header
	size_t buffer_size = 256; // 2MB buffer
	unsigned char* buffer = new unsigned char[buffer_size];
	unsigned char* packet = new unsigned char[packet_size];
	while ( true )
	{
		// if ( connection.IsConnected() )
		// {
		// 	unsigned char packet[256];
		// 	string Input;
		// 	getline(cin, Input);
		// 	memcpy(packet, Input.c_str(), sizeof(Input));
		// 	//unsigned char packet[] = "server to client";
		// 	connection.SendPacket( packet, sizeof( packet ) );
		// }
		for (int i = 0; i < buffer_size; i++){
			buffer[i] = 0;
		}
		for (int i = 0; i < packet_size; i++){
			packet[i] = 0;
		}
		size_t buffer_sequence = 0;
		while ( true )
		{
			int bytes_read = connection.ReceivePacket( packet,packet_size );
			if (!bytes_read){
				break;
			}
			unsigned char mode = packet[0] & 0xF0;
			unsigned char sequence = packet[0] & 0xF;
			unsigned char* data = &packet[1];
			cout << "receive packet: length = " << bytes_read << ",header ( mode = " << (int)mode << ",sequence = " << (int) sequence << ",body:" << endl;
			print(data, packet_size - 1);
			// if summery comes and it's not finished, then drop the packet.

			if (mode == SUMMARY && buffer_sequence < sequence){
				for (int i = 0; i < buffer_size; i++){
					buffer[i] = 0;
				}
				buffer_sequence = 0;				
				std::cerr << "Drop packet" << std::endl;
				break;
			}else if ((int)mode == FRAGMENT){
				size_t offset = sequence*(packet_size - 1);
				if (offset >= buffer_size){
					std::cerr << "Buffer Overflow" << std::endl;
					break;
				}
				memcpy(buffer+offset, data, packet_size - 1);
				buffer_sequence ++;
			}
		}
		if (buffer_sequence > 0 ){
			cout << "buffer:" << endl;
			print(buffer, buffer_sequence*(packet_size-1));
		}

		connection.Update( DeltaTime );
		wait( DeltaTime );
	}
	delete[] buffer;
	delete[] packet;
	ShutdownSockets();
	return 0;
}

int net_outchan(int port, int target_port, int ProtocolId, float DeltaTime, float TimeOut, size_t packet_size){
	if ( !InitializeSockets() )
	{
		printf( "failed to initialize sockets\n" );
		return 1;
	}

	NetworkConnection connection( ProtocolId, TimeOut );
	
	if ( !connection.Start( port ) )
	{
		printf( "could not start connection on port %d\n", port );
		return 1;
	}
	
	connection.Connect( NetworkAddress(127,0,0,1,target_port ) );
	
	bool connected = false;
	
	while ( true )
	{

		if ( !connected && connection.IsConnected() )
		{
			printf( "client connected to server\n" );
			connected = true;
		}
		
		if ( !connected && connection.ConnectFailed() )
		{
			printf( "connection failed\n" );
			break;
		}

		//Simple Input Messaging
		string input_string;
		getline(cin, input_string);
		cout << "user input: " << input_string << std::endl;
		const unsigned char* data = (unsigned char*) input_string.c_str();
		size_t data_size = input_string.size();
		unsigned char *decorated = new unsigned char[packet_size+1]; // [#frame(1 byte)][#body(n bytes)]
		unsigned char packet_index = 0;
		for (size_t i = 0; i < data_size; i += packet_size){
			size_t frame_size = (i + packet_size >= data_size) ? data_size - i : packet_size; // consider last packet
			for (size_t j = 0; j < packet_size+1; j++){ 
				decorated[j] = 0; // clean up the container
			}
			decorated[0] = FRAGMENT | packet_index; // if first coding is 1 then it's a fragment
			packet_index ++;
			std::memcpy(decorated+1, data+i, frame_size);
			connection.SendPacket(decorated, packet_size+1);
			print(decorated, packet_size+1);
		}
		for (size_t j = 0; j < packet_size+1; j++){ 
			decorated[j] = 0; // clean up the container
		}
		decorated[0] = SUMMARY | packet_index - 1;
		connection.SendPacket(decorated, packet_size+1); // send a end of current packet 
		delete[] decorated;
		connection.Update( DeltaTime );
		wait( DeltaTime );
	}
	
	ShutdownSockets();
	return 0;
}

