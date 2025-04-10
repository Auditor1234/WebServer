#pragma once
#include <memory>
#include "EventLoopThreadPool.h"

class Server {
public:
    Server(EventLoop* loop, int thread_num, int port);
    ~Server() {}

    EventLoop* getLoop() const { return loop_; }
    void start();
    void handNewConn();
    void handThisConn() { loop_->updatePoller(accept_Channel_); }

private:
    EventLoop* loop_;
    int thread_num_;
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
    bool started_;
    std::shared_ptr<Channel> accept_Channel_;
    int port_;
    int listenFd_;
    static const int MAXFDS = 100000;
};