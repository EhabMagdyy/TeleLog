#include "SafeSocket.hpp"
#include <unistd.h>

// its better to open the socket in a the higher level class (SocketTelemetrySourceImpl)
SafeSocket::SafeSocket(int sockerFd) : socketfd(sockerFd){}

SafeSocket::~SafeSocket(){
    if(socketfd != -1){
        close(socketfd);
    }
}

SafeSocket::SafeSocket(SafeSocket&& other) noexcept : socketfd(other.socketfd) {
    other.socketfd = -1;
}

SafeSocket& SafeSocket::operator=(SafeSocket&& other) noexcept {
    if(this != &other){
        if(socketfd != -1){
            close(socketfd);
        }
        socketfd = other.socketfd;
        other.socketfd = -1;
    }
    return *this;
}

int SafeSocket::getSocketFD() const{
    return socketfd;
}