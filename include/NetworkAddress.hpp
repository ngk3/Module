namespace Module {
	
	class NetworkAddress
	{
	
		public:
	
		NetworkAddress() { address = 0; port = 0; }
	
		NetworkAddress( unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port );
	
		NetworkAddress( unsigned int address, unsigned short port );
	
		unsigned int GetAddress() const { return address;}
		unsigned char GetA() const { return ( unsigned char ) ( address >> 24 );}
		unsigned char GetB() const { return ( unsigned char ) ( address >> 16 );}
		unsigned char GetC() const { return ( unsigned char ) ( address >> 8 );}
		unsigned char GetD() const { return ( unsigned char ) ( address );}
		unsigned short GetPort() const { return port;}
	
		bool operator == ( const NetworkAddress & other ) const { return address == other.address && port == other.port;}
		bool operator != ( const NetworkAddress & other ) const { return ! ( *this == other );}
		bool operator < ( const NetworkAddress & other ) const;
	
	private:
	
		unsigned int address;
		unsigned short port;
	};
}