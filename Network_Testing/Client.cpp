/*
	Client using Virtual Connection over UDP
	From "Networking for Game Programmers" - http://www.gaffer.org/networking-for-game-programmers
	Author: Glenn Fiedler <gaffer@gaffer.org>
*/
#include "NetworkChannel.cpp"

int main( int argc, char * argv[] )
{
	net_outchan(30001, 3000, PROTOCOL_ID, DELTA, TIMEOUT, 15);
	return 0;
}
