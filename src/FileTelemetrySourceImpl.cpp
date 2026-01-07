#include "FileTelemetrySourceImpl.hpp"
#include <sstream>

bool FileTelemetrySourceImpl::openSource(){
    file.emplace("/proc/stat"); // Initialize the optional with SafeFile
    if(file->getFD() == -1){
        return false;
    }
    return true;
}

bool FileTelemetrySourceImpl::readSource(std::string& out){
    char buffer[256] = {0};
    ssize_t dataBytes = read(file->getFD(), buffer, sizeof(buffer)-1);
    if(dataBytes <= 0){
        return false;
    }

    buffer[dataBytes] = '\0';
    std::string line(buffer);

    // Parse first line for CPU usage
    std::istringstream iss(line);
    std::string cpuLabel;
    unsigned long user, nice, system, idle;

    iss >> cpuLabel >> user >> nice >> system >> idle;
    if(cpuLabel != "cpu") return false;

    unsigned long total = user + nice + system + idle;
    double cpuUsage = 100.0 * (user + nice + system) / total;

    out = std::to_string(cpuUsage);

    return true;
}