#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main() {
    const char* socketPath = "/home/ehab/Documents/ITI_9Months/CppProject/sokt.sock";

    // 1️⃣ Create server socket
    int serverfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(serverfd == -1){
        perror("socket"); 
        return 1; 
    }

    // 2️⃣ Remove old socket file if it exists
    unlink(socketPath);

    // 3️⃣ Bind socket to file path
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath, sizeof(addr.sun_path)-1);

    if(bind(serverfd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
        perror("bind"); 
        close(serverfd);
        return 1; 
    }

    // 4️⃣ Listen for connections
    if(listen(serverfd, 1) == -1){
        perror("listen"); 
        close(serverfd);
        return 1; 
    }

    std::cout << "Server listening on " << socketPath << std::endl;

    // 5️⃣ Accept a client (blocks until a client connects)
    int clientfd = accept(serverfd, nullptr, nullptr);
    if(clientfd == -1){
        perror("accept"); 
        close(serverfd);
        return 1; 
    }

    std::cout << "Client connected, sending messages..." << std::endl;

    // 6️⃣ Send some messages to the client
    const char* msg1 = "Hi Socket, From Server\n";
    write(clientfd, msg1, strlen(msg1));

    // 7️⃣ Keep connection open a bit so client can read
    sleep(1);

    // 8️⃣ Close sockets and clean up
    close(clientfd);
    close(serverfd);
    unlink(socketPath);

    std::cout << "Server done." << std::endl;
    return 0;
}
