#ifndef _THREAD_H
#define _THREAD_H
#include "thread.c"

// Function prototype
int exe_thread(void *(*function_ptr)(void *), void *arg);
void thread_counter(thread_s status);
int get_active_threads(void);

#endif
