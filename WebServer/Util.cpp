#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>

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

void shutDownWR(int fd) {
    shutdown(fd, SHUT_WR);
}