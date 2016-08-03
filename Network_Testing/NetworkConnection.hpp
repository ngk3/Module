#include "NetworkSocket.cpp"
#include <cstring>

using namespace Module;

// Class that stores the connection to an NetworkAddress
namespace Module {	
	class NetworkConnection
	{
	public:
		// Different mode Connection can be in
		enum Mode
		{
			None,	
			Client,
			Server
		};
		
		//Constructor
		NetworkConnection( unsigned int protocolId, float timeout );
		
		//Destructor
		~NetworkConnection();
		
		//Starts and Stops connection via socket to a specified port
		bool Start( int port );
		void Stop();
		
		void Listen();
		void Connect( const NetworkAddress & address );
		
		// Get functions that returns the state or mode that the connection is in
		bool IsConnecting() const { return state == Connecting; }	
		bool ConnectFailed() const { return state == ConnectFail; }
		bool IsConnected() const { return state == Connected; }
		bool IsListening() const { return state == Listening; }
		Mode GetMode() const { return mode; }
		
		void Update( float deltaTime );
		bool SendPacket( const unsigned char data[], int size );
		int ReceivePacket( unsigned char data[], int size );
		
	protected:
		
		void ClearData();
	
	private:
		
		// Different states that the connection can be in
		enum State
		{
			Disconnected,
			Listening,
			Connecting,
			ConnectFail,
			Connected
		};

		// protocolID used to establish connection to a specific client or server
		unsigned int protocolId;
		// timeout = how long it takes for connection to stop and disconnect
		float timeout;
		
		// tells if the connection is running and other variables used for the NetworkConnection
		bool running;
		Mode mode;
		State state;
		NetworkSocket socket;
		float timeoutAccumulator;
		NetworkAddress address;
	};
}
