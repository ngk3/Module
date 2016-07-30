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
#include <queue>

#include "NetworkInterface.hpp"
#include <queue>
#include <pthread.h>

using namespace std;
using namespace Module;

const int PROTOCOL_ID = 0x99887766;
const float DELTA = 0.25f;
const float RATE = 0.25f;
const float TIMEOUT = 10.0f;
const unsigned char FRAGMENT = 0x80;
const unsigned char SUMMARY = 0x40;
bool NC_DEBUG = false;

class InputQueue
{
public:
	InputQueue(size_t queue_size){
		m_queue_size = queue_size;
		pthread_mutex_init(&m_mutex, NULL);
		pthread_cond_init(&m_condv, NULL);
	}
	~InputQueue(){
	    pthread_mutex_destroy(&m_mutex);
	    pthread_cond_destroy(&m_condv);
	}
	string pop(){
		pthread_mutex_lock(&m_mutex);
	    while (m_queue.size() == 0) {
	        pthread_cond_wait(&m_condv, &m_mutex);
	    }
	    string item = string(m_queue.front());
	    if (NC_DEBUG){
		    std::cout << "front poped: " << item << std::endl;
		}
	    m_queue.pop(); // bug?
	    pthread_mutex_unlock(&m_mutex);
	    return item;
	}

	void push(string info){
		pthread_mutex_lock(&m_mutex);
		m_queue.push(info);
		if (NC_DEBUG){
			std::cout << "pushed: " << info << std::endl;
		}
		if (m_queue.size() > m_queue_size){
			m_queue.pop();
		}
	    pthread_cond_signal(&m_condv);
	    pthread_mutex_unlock(&m_mutex);
	}

private:
	queue<string> m_queue;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_condv;
	size_t m_queue_size;
};

void print(unsigned char *container, size_t size){
	for (size_t i = 0; i < size; i++){
		cout << (int)(container[i]) << "|";
	}
	cout << endl;
}

string toString(unsigned char* data, size_t data_size){
	return string(data, data + data_size);
}

// inward channel, receive a dataframe and push into a priority queue
int net_inchan(int port, int ProtocolId, float DeltaTime, float TimeOut, size_t packet_size, InputQueue* queue){
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
	size_t buffer_size = 2048; // 2MB buffer
	unsigned char* buffer = new unsigned char[buffer_size];
	unsigned char* packet = new unsigned char[packet_size];
	while ( true )
	{
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
			if (NC_DEBUG){
				cout << "receive packet: length = " << bytes_read << ",header ( mode = " << (int)mode << ",sequence = " << (int) sequence << ",body:" << endl;
				print(data, packet_size - 1);
			}
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
			queue->push(toString(buffer, buffer_sequence*(packet_size-1)));
//			std::cout << toString(buffer, buffer_sequence*(packet_size-1)) << std::endl;
		}
		connection.Update( DeltaTime );
		Module::wait( DeltaTime );
	}
	delete[] buffer;
	delete[] packet;
	ShutdownSockets();
	return 0;
}

int net_outchan(int port, int target_port, int ProtocolId, float DeltaTime, float TimeOut, size_t packet_size, InputQueue& queue){
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
		string input_string = queue.pop();
		if (NC_DEBUG){
			cout << "user input: " << input_string << std::endl;
		}
		if (input_string.empty()){
			Module::wait( DeltaTime );
			continue;
		}

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
			if (NC_DEBUG){
				print(decorated, packet_size+1);
			}
		}
		for (size_t j = 0; j < packet_size+1; j++){ 
			decorated[j] = 0; // clean up the container
		}
		decorated[0] = SUMMARY | packet_index - 1;
		connection.SendPacket(decorated, packet_size+1); // send a end of current packet 
		delete[] decorated;
		connection.Update( DeltaTime );
		Module::wait( DeltaTime );
	}
	
	ShutdownSockets();
	return 0;
}


