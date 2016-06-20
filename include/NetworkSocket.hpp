//#ifndef __MODULE__NETWORKSOCKET_HPP__
//#define __MODULE__NETWORKSOCKEY_HPP__

#include "NetworkAddress.cpp"

namespace Module
{

// 
class NetworkSocket
{
	friend class NetworkInterface;

	private:
		int socket;
	
	public:
		//CONSTRUCTOR
		NetworkSocket();
	
		//DESTRUCTOR
		~NetworkSocket();
	
		//FUNCTIONS
		bool Open(unsigned short port);
		void Close();
		bool isOpen() const;
		bool send(const NetworkAddress& destination, const void* data, int size);
		int receive(NetworkAddress& sender, void* data, int size);
		
};
	
}