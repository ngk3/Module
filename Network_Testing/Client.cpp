#include "NetworkChannel.cpp"
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <sstream>
using namespace std;

void *start_outchan(void *t){
	InputQueue* q = (InputQueue*) t;	
	std::cout << "outchan started" << std::endl;
	// for (int i = 0; i < 1000; i++){
	// 	q->push("test" + static_cast<ostringstream*>( &(ostringstream() << i) )->str());
	// 	usleep(100);
	// }
	net_outchan(30001, 3000, PROTOCOL_ID, 0.01f, TIMEOUT, 15, *q);
	pthread_exit(NULL);
}

void produce_outchan(InputQueue* queue){
	int counter = 0;
	while (counter < 10){
		queue->push("test" + static_cast<ostringstream*>( &(ostringstream() << counter))->str());
		usleep(10);
		counter += 1;
	}
}

int main( int argc, char * argv[] )
{
	InputQueue* queue = new InputQueue();
	int rc;
	pthread_t threads[1];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	std::cout << "Starting threads" << std::endl;
	rc = pthread_create(&threads[0], NULL, start_outchan, (void *) queue);
	if (rc){
		std::cout << "Error: Unable to create thread" << std::endl;
		exit(-1);
	}
	produce_outchan(queue);
	pthread_join(threads[0], NULL);
	delete queue;
	return 0;
}