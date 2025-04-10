#pragma once
#include <cstdlib>

ssize_t readn(int fd, void* buf, size_t n);
ssize_t readn(int fd, std::string& inBuffer, bool& zero);
ssize_t writen(int fd, void* buf, size_t n);
ssize_t writen(int fd, std::string& outBuffer);
void shutDownWR(int fd);
void handleForSigpipe();

int setSocketNonBlocking(int fd);
int socketBindAndListen(int port);