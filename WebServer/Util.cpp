#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <strings.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string>

#include "Util.h"

const int MAX_BUFF = 4096;

ssize_t readn(int fd, void* buff, size_t n) {
    size_t nleft = n;
    ssize_t nread = 0;
    ssize_t read_sum = 0;
    char* ptr = (char*)buff;
    while(nleft > 0) {
        if((nread = read(fd, ptr, nleft)) < 0) {
            if(errno == EINTR) {
                nread = 0;
            }else if(errno == EAGAIN) {
                return read_sum;
            }else {
                return -1;
            }
        }else if(nread == 0) {
            break;
        }
        read_sum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return read_sum;
}

ssize_t readn(int fd, std::string& inBuffer, bool& zero) {
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while(true) {
        char buff[MAX_BUFF];
        if((nread = read(fd, buff, MAX_BUFF)) < 0) {
            if(errno == EINTR) {
                continue;
            }else if(errno == EAGAIN) {
                return readSum;
            }else {
                perror("Read error");
                return -1;
            }
        }else if(nread == 0) {
            if(readSum == 0)
                zero = true;
            break;
        }

        readSum += nread;
        inBuffer += std::string(buff, buff + nread);
    }
    return readSum;
}

ssize_t writen(int fd, void* buff, size_t n) {
    size_t nleft = n;
    ssize_t nwrite = 0;
    ssize_t write_sum = 0;
    char* ptr = (char*)buff;
    while(nleft > 0) {
        if((nwrite = write(fd, ptr, nleft)) <= 0) {
            if(errno == EINTR) {
                nwrite = 0;
            }else if(errno == EAGAIN) {
                return write_sum;
            }else {
                return -1;
            }
        }
        write_sum += nwrite;
        nleft -= nwrite;
        ptr += nwrite;
    }
    return write_sum;
}

ssize_t writen(int fd, std::string& outBuffer) {
    size_t nleft = outBuffer.size();
    ssize_t nwrite = 0;
    ssize_t write_sum = 0;
    const char* ptr = outBuffer.c_str();
    while(nleft > 0) {
        if((nwrite = write(fd, ptr, nleft)) <= 0) {
            if(errno == EINTR) {
                nwrite = 0;
            }else if(errno == EAGAIN) {
                return write_sum;
            }else {
                return -1;
            }
        }
        write_sum += nwrite;
        nleft -= nwrite;
        ptr += nwrite;
    }
    
    if(write_sum == static_cast<int>(outBuffer.size())) {
        outBuffer.clear();
    }else {
        outBuffer = outBuffer.substr(write_sum);
    }
    return write_sum;
}

void shutDownWR(int fd) {
    shutdown(fd, SHUT_WR);
}

int socketBindAndListen(int port) {
    if(port < 0 || port > 65535)
        return -1;
    int listen_fd = 0;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd < 0)
        return -1;
    
    int optval = 1;
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        close(listen_fd);
        return -1;
    }

    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)port);

    if(bind(listen_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        close(listen_fd);
        return -1;
    }

    if(listen(listen_fd, 2048) == -1) {
        close(listen_fd);
        return -1;
    }

    if(listen_fd == -1) {
        close(listen_fd);
        return -1;
    }
    return listen_fd;
}

int setSocketNonBlocking(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1)
        return -1;
    
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) == -1)
        return -1;
    return 0;
}

void handleForSigpipe() {
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, nullptr))
        return;
}