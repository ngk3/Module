#include "NetworkChannel.cpp"
#include <queue>
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

void consume_inchan(InputQueue* queue){
	int counter = 0;
	while (counter < 1000){
		std::cout << queue->pop() << std::endl;
		usleep(0);
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
	rc = pthread_create(&threads[0], NULL, start_inchan, (void *) queue);
	if (rc){
		std::cout << "Error: Unable to create thread" << std::endl;
		return (-1);
	}
	consume_inchan(queue);
	pthread_join(threads[0], NULL);
	delete queue;
	return 0;
}