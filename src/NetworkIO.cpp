
#include "NetworkChannel.cpp"
using namespace std;
void *start_inchan(void *t){
	InputQueue* q = (InputQueue*) t;	
	std::cout << "inchan started" << std::endl;
	// for (int i = 0; i < 1000; i++){
	// 	q->push("test" + static_cast<ostringstream*>( &(ostringstream() << i) )->str());
	// 	usleep(100);
	// }
	net_inchan(3000, PROTOCOL_ID, 0.001f, TIMEOUT, 15, q);
	pthread_exit(NULL);
}
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

void consume_inchan(InputQueue* queue){
	int counter = 0;
	while (counter < 1000){
		std::cout << queue->pop() << std::endl;
		usleep(0);
		counter += 1;
	}
}