#include "EventLoopThreadPool.h"

using namespace std;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* base_loop, int num_threads) :
    base_loop_(base_loop), started_(false), num_threads_(num_threads), next_(0)
{
    if(num_threads_ <= 0)
        abort();
}

void EventLoopThreadPool::start() {
    base_loop_->assertInLoopThread();
    started_ = true;
    for(int i = 0; i < num_threads_; i++) {
        shared_ptr<EventLoopThread> t(new EventLoopThread());
        threads_.push_back(t);
        loops_.push_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    base_loop_->assertInLoopThread();
    assert(started_);
    EventLoop* loop = base_loop_;
    if(!loops_.empty()) {
        loop = loops_[next_];
        next_ = (next_ + 1) % num_threads_;
    }
    return loop;
}