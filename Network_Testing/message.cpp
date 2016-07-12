#ifndef Message_cpp
#define Message_cpp

#include <iostream>
#include <String>
#include <cstring>
#include <cmath>
#include <bitset>
#include <vector>
#include <algorithm>

typedef long byte;
using namespace std;
class Message;
class Message{
	// a memory container for fragmentation, the memory will be immutable
public:
	Message(){}
	// pointer to data, size of data in bytes, maximum size of the message
	Message(vector<byte> data){
		this->data = data;
	}
	vector<byte> get(unsigned int start, unsigned int length){
		vector<byte> result;
		for (int i = start; i < min((long)this->data.size(), (long)start+length); i++){
			result.push_back(this->data[i]);
		}
		return result;
	}
	vector<byte> data;
};

class Fragment{
public:
	Fragment(Message message, const unsigned int fragment_size){
		this->fnum = ceil(((float)(message.data.size()))/fragment_size); this->message = message; this->fsize = fragment_size; this->fcurrent = 0;
	}
	Message message;
	unsigned int fsize;
	unsigned int fcurrent;
	unsigned int fnum;
	unsigned int size(){ return message.data.size(); }
	vector<byte> next(){
		if (fcurrent >= fnum){
			return vector<byte>();
		}		
		vector<byte> r = message.get(fcurrent*fsize,fsize);
		fcurrent ++;
		return r;
	}
	bool hasnext(){
		return fcurrent < fnum;
	}
	void dump(){
		cout << "fsize," << fsize << ",fcurrent," << fcurrent << ",fnum," << fnum << ",size," << size() << endl;
	}
};

#endif
