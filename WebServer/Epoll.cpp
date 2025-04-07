#include <iostream>
#include <assert.h>
#include "Epoll.h"

using namespace std;

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

Epoll::Epoll() : epollFd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM) {
    assert(epollFd_ > 0);
}
Epoll::~Epoll() {}

void Epoll::epollAdd(SP_Channel request, int timeout) {
    int fd = request->getFd();
    if(timeout > 0) {
        addTimer(request, timeout);
        fd2http_[fd] = request->getHolder();
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    
    request->equalAndUpdateLastEvents();

    fd2chan_[fd] = request;
    if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0) {
        perror("epoll_add error");
        fd2chan_[fd].reset();
    }
}
void Epoll::epollMod(SP_Channel request, int timeout) {
    int fd = request->getFd();
    if(timeout > 0) {
        addTimer(request, timeout);
    }

    if(!request->equalAndUpdateLastEvents()) {
        struct epoll_event event;
        event.data.fd = fd;
        event.events = request->getEvents();
        if(epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0) {
            perror("epoll_mod error");
            fd2chan_[fd].reset();
        }
    }
}

void Epoll::epollDel(SP_Channel request) {
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getLastEvents();
    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0) {
        perror("epoll_del_error");
    }
    fd2chan_[fd].reset();
    fd2http_[fd].reset();
}

vector<SP_Channel> Epoll::poll() {
    while(true) {
        int event_count = epoll_wait(epollFd_, &*events_.begin(), events_.size(), EPOLLWAIT_TIME);
        if(event_count < 0)
            perror("epoll_wait error");
        vector<SP_Channel> req_data = getEventsRequest(event_count);
        if(req_data.size() > 0)
            return req_data;
    }
}

vector<SP_Channel> Epoll::getEventsRequest(int events_num) {
    vector<SP_Channel> req_data;
    for(int i = 0; i < events_num; i++) {
        int fd = events_[i].data.fd;
        SP_Channel cur_req = fd2chan_[fd];

        if(cur_req) {
            cur_req->setRevents(events_[i].events);
            cur_req->setEvents(0);

            req_data.push_back(cur_req);
        }else {
            cout << "SP cur_req is invalid" << endl;
        }
    }
    return req_data;
}

void Epoll::addTimer(SP_Channel request_data, int timeout) {
    shared_ptr<HttpData> t = request_data->getHolder();
    if(t) {
        cout << "Timer add success" << endl;
    }else {
        cout << "Timer add fail" << endl;
    }
}

void Epoll::handleExpired() {
    
}
