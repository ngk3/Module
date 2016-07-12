#include "NetworkChannel.cpp"
int main( int argc, char * argv[] )
{
	net_inchan(3000, PROTOCOL_ID, DELTA, TIMEOUT, 15);
	return 0;
}