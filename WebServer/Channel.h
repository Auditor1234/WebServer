#pragma once
#include <functional>
#include <memory>
#include <sys/epoll.h>

class EventLoop;
class HttpData;

class Channel {
private:
    using CallBack = std::function<void()>;
    EventLoop* loop_;
    int fd_;
    __uint32_t events_;
    __uint32_t revents_;
    __uint32_t last_events_;

    std::weak_ptr<HttpData> holder_;

private:
    int parseURI();
    int parseHeaders();
    int analyseRequest();

    CallBack read_handler_;
    CallBack write_handler_;
    CallBack error_handler_;
    CallBack conn_handler_;

public:
    Channel(EventLoop* loop);
    Channel(EventLoop* loop, int fd);
    ~Channel();
    int getFd();
    void setFd(int fd);

    void setHolder(std::shared_ptr<HttpData> holder) { holder_ = holder; }
    std::shared_ptr<HttpData> getHolder() {
        std::shared_ptr<HttpData> ret(holder_.lock());
        return ret;
    }

    void setReadHandler(CallBack&& read_handler) { read_handler_ = read_handler; }
    void setWriteHandler(CallBack&& write_handler) { write_handler_ = write_handler; }
    void setErrorHandler(CallBack&& error_handler) { error_handler_ = error_handler; }
    void setConnHandler(CallBack&& conn_handler) { conn_handler_ = conn_handler; }

    void handleEvents() {
        events_ = 0;
        if((revents_ & EPOLLHUP) && !(revents_ && EPOLLIN)) {
            events_ = 0;
            return;
        }
        if(revents_ & EPOLLERR) {
            if(error_handler_)
                error_handler_();
            events_ = 0;
            return;
        }
        if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
            handleRead();
        }
        handleConn();
    }

    void handleRead();
    void handleWrite();
    void handleError(int fd, int err_num, std::string short_msg);
    void handleConn();

    void setRevents(__uint32_t ev) { revents_ = ev; }
    void setEvents(__uint32_t ev) { events_ = ev; }
    __uint32_t& getEvents() { return events_; }

    bool equalAndUpdateLastEvents() {
        bool ret = (last_events_ == events_);
        last_events_ = events_;
        return ret;
    }

    __uint32_t getLastEvents() { return last_events_; }
};

using SP_Channel = std::shared_ptr<Channel>;