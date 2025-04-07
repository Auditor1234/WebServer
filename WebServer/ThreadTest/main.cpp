#include <iostream>
#include <unistd.h>
#include <functional>
#include "../base/Thread.h"

using namespace std;

void printHello() {
    cout << "Hello world, I am thread: " << gettid() << endl;
}

int main(int argc, char* argv[]) {
    Thread th1(bind(&printHello), "First thread");
    Thread th2(bind(&printHello), "Second thread");

    th1.start();
    th2.start();
    return 0;
}