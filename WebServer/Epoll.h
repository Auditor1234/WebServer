#pragma once
#include <sys/epoll.h>
#include <memory>
#include <vector>
#include "Channel.h"

class Epoll {
public:
    Epoll();
    ~Epoll();
    void epollAdd(SP_Channel request, int timeout);
    void epollMod(SP_Channel request, int timeout);
    void epollDel(SP_Channel request);
    std::vector<SP_Channel> poll();
    std::vector<SP_Channel> getEventsRequest(int events_num);
    void addTimer(SP_Channel request_data, int timeout);
    int getEpollFd() { return epollFd_; }
    void handleExpired();

private:
    static const int MAXFDS = 100000;
    int epollFd_;
    std::vector<epoll_event> events_;
    SP_Channel fd2chan_[MAXFDS];
    std::shared_ptr<HttpData> fd2http_[MAXFDS];
};