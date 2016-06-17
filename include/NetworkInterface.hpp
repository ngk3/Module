#ifndef __MODULE__NETWORKINTERFACE_HPP__
#define __MODULE__NETWORKINTERFACE_HPP__

// Platform Detection

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
/*
#include <assert.h>
#include <vector>
#include <map>
#include <stack>
#include <cstring>

#include <EASTL/string.h>
#include <EASTL/vector.h>
*/

// Module includes
#include "ModuleDefines.hpp"

namespace Module
{
/** The NetworkInterface is ...
 */
 
class NetworkInterface
{
	// 
	friend class Server;
	friend class Client;
	friend class ModuleGame;
	
	protected:
		// REPRESENTATION
		ModuleGame* game;		/**< The game that this is a part of */
	
}
	
}