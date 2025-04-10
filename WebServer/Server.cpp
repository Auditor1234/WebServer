#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include "Server.h"
#include "HttpData.h"

Server::Server(EventLoop* loop, int thread_num, int port) :
    loop_(loop),
    thread_num_(thread_num),
    eventLoopThreadPool_(new EventLoopThreadPool(loop_, thread_num)),
    started_(false),
    accept_Channel_(new Channel(loop_)),
    port_(port),
    listenFd_(socketBindAndListen(port))
{
    accept_Channel_->setFd(listenFd_);
    handleForSigpipe();
    if(setSocketNonBlocking(listenFd_) < 0) {
        perror("Set socket non block failed");
        abort();
    }
}

void Server::start() {
    eventLoopThreadPool_->start();
    accept_Channel_->setEvents(EPOLLIN | EPOLLET);
    accept_Channel_->setReadHandler(bind(&Server::handNewConn, this));
    accept_Channel_->setConnHandler(bind(&Server::handThisConn, this));
    loop_->addToPoller(accept_Channel_, 0);
    started_ = true;
}

void Server::handNewConn() {
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd = 0;

    while((accept_fd = accept(listenFd_, (struct sockaddr*)&client_addr, &client_addr_len)) > 0) {
        EventLoop* loop = eventLoopThreadPool_->getNextLoop();
        cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;

        if(accept_fd >= MAXFDS) {
            close(accept_fd);
            continue;
        }

        if(setSocketNonBlocking(accept_fd) < 0) {
            perror("Set non block failed!");
            return;
        }

        // 这里主线程跑去处理了这个连接，导致子线程没有数据可以读取
        shared_ptr<HttpData> req_info(new HttpData(loop, accept_fd));
        req_info->getChannel()->setHolder(req_info);
        loop->queueInLoop(bind(&HttpData::newEvent, req_info));
    }
    accept_Channel_->setEvents(EPOLLIN | EPOLLET);
}