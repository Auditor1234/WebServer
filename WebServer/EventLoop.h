#pragma once
#include <functional>
#include <memory>
#include <iostream>
#include <assert.h>
#include "Epoll.h"
#include "base/CurrentThread.h"
#include "base/MutexLock.h"
#include "Util.h"

class EventLoop {
public:
    using Functor = std::function<void()>;
    EventLoop();
    ~EventLoop();
    
    void loop();
    void quit();
    void runInLoop(Functor&& cb);
    void queueInLoop(Functor&& cb);
    bool isInLoopThread() const { return thread_id_ == CurrentThread::tid(); }
    void assertInLoopThread() { assert(isInLoopThread()); }
    void shutdown(SP_Channel channel) { shutDownWR(channel->getFd()); }
    void removeFromPoller(SP_Channel channel) {
        poller_->epollDel(channel);
    }
    void updatePoller(SP_Channel channel, int timeout = 0) {
        poller_->epollMod(channel, timeout);
    }
    void adddToPoller(SP_Channel channel, int timeout = 0) {
        poller_->epollAdd(channel, timeout);
    }

private:
    bool looping_;
    std::shared_ptr<Epoll> poller_;
    int wakeupFd_;
    bool quit_;
    bool event_handling_;
    mutable MutexLock mutex_;
    std::vector<Functor> pending_Functors_;
    bool calling_pending_Functors_;
    const pid_t thread_id_;
    SP_Channel pwakeup_Channel_;

    void wakeup();
    void handleRead();
    void doPendingFunctors();
    void handleConn();
};