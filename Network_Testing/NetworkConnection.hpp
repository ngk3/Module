#include "NetworkSocket.cpp"
#include <cstring>

using namespace Module;

namespace Module {	
	class NetworkConnection
	{
	public:
		
		enum Mode
		{
			None,	
			Client,
			Server
		};
		
		NetworkConnection( unsigned int protocolId, float timeout );
		~NetworkConnection();
		
		bool Start( int port );
		void Stop();
		
		void Listen();
		void Connect( const NetworkAddress & address );
		
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
		
		enum State
		{
			Disconnected,
			Listening,
			Connecting,
			ConnectFail,
			Connected
		};

		unsigned int protocolId;
		float timeout;
		
		bool running;
		Mode mode;
		State state;
		NetworkSocket socket;
		float timeoutAccumulator;
		NetworkAddress address;
	};
}