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
    const char* address = "127.0.0.1";

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0) {
        perror("Socket create error");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, address, &server_addr.sin_addr);

    if(connect(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect error");
        return -1;
    }

    const char* data = "Hello, I am client.";
    setSocketNonBlocking(socket_fd);
    ssize_t n = write(socket_fd, data, strlen(data));
    cout << n << " bytes has been sent sucessfully." << endl;
    sleep(1);

    char buff[4096];
    bzero(buff, sizeof(buff));
    n = read(socket_fd, buff, sizeof(buff));
    cout << "Get " << n << " bytes data:" << endl;
    cout << buff << endl;
    close(socket_fd);
}