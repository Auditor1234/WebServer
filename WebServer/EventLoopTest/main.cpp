#include <iostream>
#include <unistd.h>
#include <functional>
#include "../base/Thread.h"
#include "../EventLoop.h"

using namespace std;

void printHello() {
    cout << "Hello world, I am thread: " << gettid() << endl;
}

int main(int argc, char* argv[]) {
    EventLoop event_loop;
    for(int i = 0; i < 100; i++) {
        event_loop.queueInLoop(bind(&printHello));
    }
    // must wakeup, or task will not be executed
    // event_loop.wakeup();
    event_loop.loop();
    return 0;
}