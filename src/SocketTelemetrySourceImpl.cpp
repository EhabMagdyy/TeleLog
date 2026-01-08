#include "SocketTelemetrySourceImpl.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>

bool SocketTelemetrySourceImpl::openSource(){
    socket.reset(); // Ensure no previous socket is open

    int socketfd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if(socketfd == -1){
        return false;
    }
    // For Unix Domain sockets, the address is a filesystem path (not IP/port).
    sockaddr_un serverAddr;
    serverAddr.sun_family = AF_UNIX;
    // filesystem path for the socket
    strncpy(serverAddr.sun_path, "/home/ehab/Documents/ITI_9Months/CppProject/sokt.sock", sizeof(serverAddr.sun_path)-1);
    serverAddr.sun_path[sizeof(serverAddr.sun_path)-1] = '\0';
    if(connect(socketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1){
        close(socketfd);
        return false;
    }

    socket.emplace(socketfd);
    return true;
}

bool SocketTelemetrySourceImpl::readSource(std::string& out){
    int bytesRead = 0;
    if(!socket.has_value() || socket->getSocketFD() == -1){
        return false;
    }

    out.resize(128);
    bytesRead = read(socket->getSocketFD(), out.data(), out.size());
    if(bytesRead <= 0){
        return false;
    }
    out.resize(bytesRead);
    return true;
}