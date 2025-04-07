#include "EventLoopThread.h"

EventLoopThread::EventLoopThread() :
    loop_(nullptr),
    exiting_(false),
    thread_(bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
    mutex_(),
    cond_(mutex_) {}

EventLoopThread::~EventLoopThread() {
    cout << "loop_ != nullptr" << (loop_ != nullptr) << endl;
    exiting_ = true;
    if(loop_ != nullptr) {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();
    {
        MutexLockGuard lock(mutex_);
        while(loop_ == nullptr) // 主线程会在这里等待，直到子线程创建好loop_变量
            cond_.wait();
    }
    return loop_;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }

    loop.loop();
    cout << "loop_ = nullptr" << endl;
    loop_ = nullptr;
}