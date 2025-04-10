#pragma once
#include <memory>
#include <unistd.h>
#include "EventLoop.h"

class Channel;

class HttpData : public std::enable_shared_from_this<HttpData> {
public:
    HttpData(EventLoop* loop, int connfd);
    ~HttpData() { close(fd_); }
    void reset();
    void seperateTimer();
    void linkTimer();
    SP_Channel getChannel() { return channel_; }
    EventLoop* getLoop() { return loop_; }
    void handleClose();
    void newEvent();

public:
    void handleRead();
    void handleWrite();
    void handleConn();
    void handleError(int fd, int err_num, std::string short_msg);

private:
    EventLoop* loop_;
    SP_Channel channel_;
    int fd_;
    std::string inBuffer_;
    std::string outBuffer_;
    bool error_;
};