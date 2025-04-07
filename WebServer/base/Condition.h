#pragma once
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include "Condition.h"
#include "noncopyable.h"
#include "MutexLock.h"

class Condition : noncopyable {
public:
    Condition(MutexLock& mutexLock) : mutexLock_(mutexLock) { pthread_cond_init(&cv_, nullptr); }
    ~Condition() { pthread_cond_destroy(&cv_); }
    
    void wait() { pthread_cond_wait(&cv_, mutexLock_.get()); }
    void notify() { pthread_cond_signal(&cv_); }
    void notifyAll() { pthread_cond_broadcast(&cv_); }
    bool waitForSeconds(int seconds) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<int>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&cv_, mutexLock_.get(), &abstime);
    }

private:
    MutexLock& mutexLock_;
    pthread_cond_t cv_;
};