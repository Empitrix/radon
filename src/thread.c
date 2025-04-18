#include <pthread.h>


static pthread_mutex_t _counter_mutex = PTHREAD_MUTEX_INITIALIZER;
static int _active_threads = 0;


typedef enum {
	START_THREAD_FUNCTION = 0,
	END_THREAD_FUNCTION   = 1,
} thread_s ;




void thread_counter(thread_s status){
	pthread_mutex_lock(&_counter_mutex);
	_active_threads = _active_threads + (status == START_THREAD_FUNCTION ? +1 : -1);
	pthread_mutex_unlock(&_counter_mutex);
}


int get_active_threads(void){ return _active_threads; }


// Function that runs the given function pointer in a new thread
int exe_thread(void *(*function_ptr)(void *), void *arg) {
	pthread_t thread;
	int result = pthread_create(&thread, NULL, function_ptr, arg);
	pthread_detach(thread);
	return result;
}

