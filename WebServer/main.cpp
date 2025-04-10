#include <iostream>
#include <unistd.h>
#include <functional>
#include "base/Thread.h"
#include "EventLoopThreadPool.h"
#include "Server.h"

using namespace std;

void printHello() {
    cout << "Hello world, I am thread: " << gettid() << endl;
}

int main(int argc, char* argv[]) {
    int thread_num = 4;
    int port = 8888;
    
    EventLoop main_loop;
    Server http_server(&main_loop, thread_num, port);
    http_server.start();
    main_loop.loop();

    return 0;
}