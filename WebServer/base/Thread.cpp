#include <unistd.h>
#include <syscall.h>
#include <stdio.h>
#include <assert.h>
#include <sys/prctl.h>
#include <iostream>
#include "CurrentThread.h"
#include "Thread.h"

using namespace std;

namespace CurrentThread {
    __thread int t_cached_tid = 0;
    __thread char t_tid_string[32];
    __thread int t_tid_string_length = 7;
    __thread const char* t_thread_name = "default";
}
pid_t gettid() { return static_cast<pid_t>(syscall(SYS_gettid)); }

void CurrentThread::cacheTid() {
    if (t_cached_tid == 0) {
        t_cached_tid = gettid();
        t_tid_string_length=
            snprintf(t_tid_string, sizeof(t_tid_string), "%5d ", t_cached_tid);
    }
}

struct ThreadData {
    typedef Thread::ThreadFunc ThreadFunc;
private:
    ThreadFunc func_;
    string name_;
    pid_t* tid_;
public:
    ThreadData(const ThreadFunc& func, const string& name, pid_t* tid) :
        func_(func), name_(name), tid_(tid) {}
    
    void runInThread() {
        *tid_ = CurrentThread::tid();
        // tid_ = nullptr;

        CurrentThread::t_thread_name = name_.empty() ? "Thread" : name_.c_str();
        prctl(PR_SET_NAME, CurrentThread::t_thread_name);

        func_();
        CurrentThread::t_thread_name = "finished";
    }
};

void* startThread(void* obj) {
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return nullptr;
}

Thread::Thread(const ThreadFunc& func, const string& name) :
    func_(func),
    name_(name),
    started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0)
{
    setDefaultName();
}

Thread::~Thread() {
    if(started_ && !joined_)
        pthread_detach(pthreadId_);
}

void Thread::start() {
    assert(!started_);
    started_ = true;
    ThreadData* data = new ThreadData(func_, name_, &tid_);
    if(pthread_create(&pthreadId_, nullptr, &startThread, data)) { // return 0 if success
        started_ = false;
        delete data;
    }else {
        sleep(1); // 保证新线程执行完毕获得tid_的值
        cout << "tid_ = " << tid_ << endl; // 这里是创建的新线程的id
        cout << "My tid = " << syscall(SYS_gettid) << endl; // 这里是主线程的id
        cout << "pthreadId_ = " << pthreadId_ << endl; // 这个值不是线程id
        assert(tid_ > 0);
    }
}

int Thread::join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, nullptr);
}

void Thread::setDefaultName() {
    if(name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread");
        name_ = buf;
    }
}
