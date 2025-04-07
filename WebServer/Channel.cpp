#include "Channel.h"

Channel::Channel(EventLoop* loop) :
    loop_(loop),
    fd_(0),
    events_(0),
    last_events_(0) {}
Channel::Channel(EventLoop* loop, int fd) :
    loop_(loop),
    fd_(fd),
    events_(0),
    last_events_(0) {}

Channel::~Channel() {}

int Channel::getFd() { return fd_; }
void Channel::setFd(int fd) { fd_ = fd; }


void Channel::handleRead() {
    if(read_handler_)
        read_handler_();
}
void Channel::handleWrite() {
    if(write_handler_)
        write_handler_();
}
void Channel::handleConn() {
    if(conn_handler_)
        conn_handler_();
}
