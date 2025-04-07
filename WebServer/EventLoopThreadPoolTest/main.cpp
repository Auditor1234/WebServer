#include <iostream>
#include <unistd.h>
#include <functional>
#include "../base/Thread.h"
#include "../EventLoop.h"
#include "../EventLoopThreadPool.h"

using namespace std;

void printHello() {
    cout << "Hello world, I am thread: " << gettid() << endl;
}

int main(int argc, char* argv[]) {
    EventLoop base_loop;
    EventLoopThreadPool eventLoopThreadPool(&base_loop, 4);
    eventLoopThreadPool.start();
    for(int i = 0; i < 100; i++) {
        EventLoop* next_loop = eventLoopThreadPool.getNextLoop();
        next_loop->queueInLoop(bind(&printHello));
    }
    cout << "loop start." << endl;
    base_loop.loop();
    return 0;
}