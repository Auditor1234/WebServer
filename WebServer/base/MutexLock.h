#pragma once
#include <pthread.h>
#include "noncopyable.h"

class MutexLock : noncopyable {
public:
    MutexLock() { pthread_mutex_init(&mutex_, nullptr); }
    ~MutexLock() {
        pthread_mutex_destroy(&mutex_);
    }

    void lock() { pthread_mutex_lock(&mutex_); }
    void unlock() { pthread_mutex_unlock(&mutex_); }
    pthread_mutex_t* get() { return &mutex_; }

private:
    pthread_mutex_t mutex_;
};

class MutexLockGuard : noncopyable {
public:
    MutexLockGuard(MutexLock &mutexLock) : mutexLock_(mutexLock){ mutexLock_.lock(); }
    ~MutexLockGuard() { mutexLock_.unlock(); }
private:
    MutexLock& mutexLock_;
};