#include <sys/eventfd.h>
#include <unistd.h>
#include <memory>
#include "EventLoop.h"

using namespace std;

__thread EventLoop* t_loop_in_this_thread = nullptr;

int createEventfd() {
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0) {
        perror("Failed in eventfd");
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop() :
    looping_(false),
    poller_(new Epoll()),
    wakeupFd_(createEventfd()),
    quit_(false),
    event_handling_(false),
    calling_pending_Functors_(false),
    thread_id_(CurrentThread::tid()),
    pwakeup_Channel_(new Channel(this, wakeupFd_))
{
    if(t_loop_in_this_thread) {

    }else {
        t_loop_in_this_thread = this;
    }

    pwakeup_Channel_->setEvents(EPOLLIN | EPOLLET);
    pwakeup_Channel_->setReadHandler(bind(&EventLoop::handleRead, this));
    pwakeup_Channel_->setConnHandler(bind(&EventLoop::handleConn, this));
    poller_->epollAdd(pwakeup_Channel_, 0);
}

EventLoop::~EventLoop() {
    close(wakeupFd_);
    t_loop_in_this_thread = nullptr;
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = writen(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one)) {
        perror("EventLoop::wakeup() reads error");
    }
}

void EventLoop::handleConn() {
    updatePoller(pwakeup_Channel_, 0);
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = readn(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one)) {
        perror("EventLoop::handleRead() reads error");
    }
    pwakeup_Channel_->setEvents(EPOLLIN | EPOLLET);
}

void EventLoop::runInLoop(Functor&& cb) {
    if(isInLoopThread()) {
        cb();
    }else {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor&& cb) {
    {
        MutexLockGuard lock(mutex_);
        pending_Functors_.emplace_back(std::move(cb));
    }
    //添加的计算任务队列后，是否唤醒目标进程分三种情况：
    // 1.当前执行的线程不是目标线程则需要唤醒目标线程；
    // 2.当前线程就是目标线程但是正处于处理计算任务中则需要唤醒进程；
    // 3.在当前线程中且正在处理回调函数，不需要唤醒线程，因为处理完回调函数后紧接着就处理计算任务队列了
    // 只有调用线程不是loop所属线程，并且calling_pending_Functors_为false时不需要唤醒
    if(!isInLoopThread() || calling_pending_Functors_)
        wakeup();
}

void EventLoop::loop() {
    assert(!looping_);
    assert(isInLoopThread());
    looping_ = true;
    quit_ = false;
    vector<SP_Channel> ret;
    while(!quit_) {
        ret.clear();
        ret = poller_->poll();
        event_handling_ = true;
        for(auto& it : ret)
            it->handleEvents();
        event_handling_ = false;
        doPendingFunctors();
        poller_->handleExpired();
    }
    looping_ = false;
}

void EventLoop::doPendingFunctors() {
    vector<Functor> functors;
    calling_pending_Functors_ = true;

    {
        MutexLockGuard lock(mutex_);
        functors.swap(pending_Functors_);
    }
    for(size_t i = 0; i < functors.size(); i++)
        functors[i]();
    calling_pending_Functors_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if(!isInLoopThread())
        wakeup();
}