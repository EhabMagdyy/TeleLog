#include <sstream>
#include <unistd.h>
#include <fcntl.h>

constexpr const char* TELEMETRY_FILE_PATH = "/home/ehab/Documents/ITI_9Months/CppProject/file.txt";

int main(){
    // 1. Open /proc/stat & Read CPU usage
    int fd = open("/proc/stat", O_RDONLY);
    if(fd == -1){
        return 1;
    }
    char buffer[256] = {0};
    ssize_t dataBytes = read(fd, buffer, sizeof(buffer)-1);
    if(dataBytes <= 0){
        close(fd);
        return 1;
    }
    buffer[dataBytes] = '\0';
    std::string line(buffer);
    // 2. Parse first line for CPU usage percentage
    std::istringstream iss(line);
    std::string cpuLabel;
    unsigned long user, nice, system, idle; 
    iss >> cpuLabel >> user >> nice >> system >> idle;
    if(cpuLabel != "cpu"){
        close(fd);
        return 1;
    }
    unsigned long total = user + nice + system + idle;
    double cpuUsage = 100.0 * (user + nice + system) / total;
    
    // 3. Write CPU usage to a file
    std::string outputFile = TELEMETRY_FILE_PATH;
    int outFd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(outFd == -1){
        return 1;
    }
    std::string cpuUsageStr = std::to_string(cpuUsage);
    write(outFd, cpuUsageStr.c_str(), cpuUsageStr.size());

    close(outFd);
    close(fd);
    
    return 0;
}
