#include <iostream>
#include <unistd.h>
#include <functional>
#include <memory>
#include "../base/Thread.h"
#include "../EventLoopThread.h"

using namespace std;

void printHello() {
    cout << "Hello world, I am thread: " << gettid() << endl;
}

int main(int argc, char* argv[]) {
    EventLoopThread event_loop_thread;
    EventLoop* event_loop = event_loop_thread.startLoop();
    // 这个地方不能用智能指针，具体原因不明
    // shared_ptr<EventLoop> event_loop(event_loop_thread.startLoop());
    for(int i = 0; i < 30; i++) {
        event_loop->queueInLoop(bind(&printHello));
    }
    return 0;
}