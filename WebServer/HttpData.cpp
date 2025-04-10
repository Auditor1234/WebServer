#include "HttpData.h"
#include <fcntl.h>

using namespace std;

// 默认边缘触发读事件，只通知一次
const __uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET | EPOLLONESHOT;
const int DEFAULT_EXPIRED_TIME = 2000; // 2秒过期

HttpData::HttpData(EventLoop* loop, int connfd) :
    loop_(loop),
    channel_(new Channel(loop, connfd)),
    fd_(connfd),
    error_(false)
{
    channel_->setReadHandler(bind(&HttpData::handleRead, this));
    channel_->setWriteHandler(bind(&HttpData::handleWrite, this));
    channel_->setConnHandler(bind(&HttpData::handleConn, this));
}

void HttpData::handleRead() {
    bool zero = false;
    int read_num = readn(fd_, inBuffer_, zero);
    if(read_num < 0) {
        perror("Read failed");
        error_ = true;
        handleError(fd_, 400, "Bad Request");
    }else if(zero) {
        // 有请求但是读不到数据
        cout << gettid() << " read no data" << endl;
        // outBuffer_ = "I am server, but read no data.";
    }
    if(!error_) {
        outBuffer_ = "Server has read successfully.\n";
        handleWrite();
    }
}

void HttpData::handleWrite() {
    if(!error_) {
        __uint32_t& events_ = channel_->getEvents();
        cout << "Current handleWrite tid = " << gettid() << ", events_ = " << events_ << endl;
        ssize_t write_num = 0;
        if((write_num = writen(fd_, outBuffer_)) < 0) {
            perror("Written");
            events_ = 0;
            error_ = true;
        }else {
            cout << "Written success: " << write_num << endl;
        }
        if(outBuffer_.size() > 0)
            events_ |= EPOLLOUT;
    }
}


void HttpData::handleConn() {
    if(error_)  {
        loop_->runInLoop(bind(&HttpData::handleClose, shared_from_this()));
    }
}

void HttpData::handleError(int fd, int err_num, string short_msg) {
    short_msg = "Server handle error: " + short_msg;
    ssize_t char_num = writen(fd, short_msg);
    cout << "Sende error " << char_num << " chars." << endl;
}

void HttpData::handleClose() {
    shared_ptr<HttpData> guard(shared_from_this());
    loop_->removeFromPoller(channel_);
}


void HttpData::newEvent() {
    channel_->setEvents(DEFAULT_EVENT);
    loop_->addToPoller(channel_, DEFAULT_EXPIRED_TIME);
}