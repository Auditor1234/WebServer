#pragma once
#include "EventLoopThread.h"

class EventLoopThreadPool : noncopyable {
public:
    EventLoopThreadPool(EventLoop* base_loop, int num_threads);
    ~EventLoopThreadPool() {
        cout << "~EventLoopThreadPool()" << endl;
    }

    void start();
    EventLoop* getNextLoop();

private:
    EventLoop* base_loop_;
    bool started_;
    int num_threads_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};