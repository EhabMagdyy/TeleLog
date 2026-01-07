#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

unsigned long parseMemInfo(const std::string& keyToFind) {
    std::ifstream memFile("/proc/meminfo");
    if(!memFile.is_open()) return 0;

    std::string line;
    while (std::getline(memFile, line)) {
        std::istringstream iss(line);
        std::string key;
        unsigned long value;
        std::string unit;
        if (!(iss >> key >> value >> unit)) continue; // skip malformed lines
        if (key == keyToFind)
            return value;
    }
    return 0;
}


int main() {
    // Get memory usage
    unsigned long totalMem = parseMemInfo("MemTotal:");
    unsigned long memAvailable = parseMemInfo("MemAvailable:");

    if (totalMem == 0) {
        std::cerr << "Failed to read MemTotal\n";
        return 1;
    }

    double memUsage = 100.0 * (totalMem - memAvailable) / totalMem;

    // Create server socket
    int serverfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(serverfd == -1){
        perror("socket"); 
        return 1; 
    }

    const char* socketPath = "/home/ehab/Documents/ITI_9Months/CppProject/sokt.sock";
    // Remove old socket file if it exists
    unlink(socketPath);

    // Bind socket to file path
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath, sizeof(addr.sun_path)-1);

    if(bind(serverfd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
        perror("bind"); 
        close(serverfd);
        return 1; 
    }

    // Listen for connections
    if(listen(serverfd, 1) == -1){
        perror("listen"); 
        close(serverfd);
        return 1; 
    }

    std::cout << "Server listening on " << socketPath << std::endl;

    // Accept a client (blocks until a client connects)
    int clientfd = accept(serverfd, nullptr, nullptr);
    if(clientfd == -1){
        perror("accept"); 
        close(serverfd);
        return 1; 
    }

    std::cout << "Client connected, sending messages..." << std::endl;

    // Send some messages to the client
    std::string msg1 = std::to_string(memUsage);
    write(clientfd, msg1.c_str(), msg1.size());

    // Keep connection open a bit so client can read
    sleep(1);

    // Close sockets and clean up
    close(clientfd);
    close(serverfd);
    unlink(socketPath);

    std::cout << "Server done." << std::endl;
    return 0;
}
