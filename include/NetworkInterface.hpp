//#ifndef __MODULE__NETWORKINTERFACE_HPP__
//#define __MODULE__NETWORKINTERFACE_HPP__

// Platform Detection and Includes

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
	#include <winsock2.h>
	#pragma comment( lib, "wsock32.lib" )

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <fcntl.h>

#else
	#error unknown platform!

#endif

// C++ standard library includes


// Module includes
/*#include "ModuleDefines.hpp"
#include "ThreadObject.hpp"
#include "NetworkCallback.hpp"
#include "NetSocket.hpp"
*/
namespace Module
{
/** The NetworkInterface is ...
 */
 
class NetworkInterface
{	
	//friend class ModuleGame;
	friend class NetworkSocket;
	
	protected:
		int server_port;

	public:
		//CONSTRUCTOR
		NetworkInterface();
	
		//FUNCTIONS
		
		virtual void openServerUDP(unsigned short port); //Opens a UDP server port with a given port number
		virtual void closeServerUDP(unsigned short port); //Closes a UDP server port with a given port number
		/*
		virtual void connectClientUDP(unsigned short clientPort, unsigned short distantPort); //Connects a client Port to a server Port
		virtual void disconnectClientUDP(unsigned short clientPort, unsigned short distantPort); //Disconnects a client given the port parameters
		*/
		/*
		void attachCallbackClient();
		void attachCallbackServer();
		*/	
	
};
	
}