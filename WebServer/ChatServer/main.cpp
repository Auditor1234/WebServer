#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>

using namespace std;

int setSocketNonBlocking(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1)
        return flag;
    
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) < 0)
        return -1;
    return 0;
}

int main(int argc, char* argv[]) {
    unsigned int port = 8888;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0) {
        perror("Socket create error");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(socket_fd);
        return -1;
    }

    if(listen(socket_fd, 1024)) {
        perror("listen");
        close(socket_fd);
        return -1;
    }

    int accept_fd = 0;
    socklen_t len = sizeof(server_addr);
    while((accept_fd = accept(socket_fd, (sockaddr*)&server_addr, &len)) > 0) {
        char buf[1024];
        bzero(buf, sizeof(buf));
        cout << "sizeof(buf) = " << sizeof(buf) << endl;
        int n = read(accept_fd, buf, sizeof(buf));
        if(n < 0) {
            perror("Read");
            close(socket_fd);
            return -1;
        }
        cout << "Accept data: " << buf << endl;
        string feedback = "Server has received sucessfully.\n";
        const char* ptr = feedback.c_str();
        n = write(accept_fd, ptr, strlen(ptr) + 1);
        if(n < 0) {
            perror("Write");
            close(accept_fd);
            return -1;
        }else {
            cout << n << " bytes has been written successfully" << endl;
        }
    }
}