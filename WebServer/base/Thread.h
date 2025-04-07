#pragma once
#include <functional>
#include "noncopyable.h"

using namespace std;

class Thread : noncopyable {
public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc&, const string& name = std::string());
    ~Thread();

    void start();
    int join();
    bool started() const { return started_; }
    pid_t tid() const { return tid_; }
    const std::string& getName() { return name_; }

private:
    void setDefaultName();

private:
    ThreadFunc func_;
    std::string name_;
    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
};