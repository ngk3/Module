// Customized NetworkSocket class

#include <assert.h>

using namespace Module;

namespace Module {
	
	// Wait function implemented depending on platform used
	#if PLATFORM == PLATFORM_WINDOWS

	void wait( float seconds )
	{
		Sleep( (int) ( seconds * 1000.0f ) );
	}

	#else

	#include <unistd.h>
	void wait( float seconds ) { usleep( (int) ( seconds * 1000000.0f ) ); }

	#endif
	
	
	class NetworkSocket
	{
	public:
	
		NetworkSocket() { socket = 0; }
		~NetworkSocket() { Close(); }
	
		bool Open( unsigned short port );
		void Close();
	
		bool IsOpen() const { return socket != 0; }
	
		bool Send( const NetworkAddress & destination, const void * data, int size );
		int Receive( NetworkAddress & sender, void * data, int size );
		
	private:
	
		int socket;
	};
}
