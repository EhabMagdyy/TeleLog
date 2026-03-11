#include "FileTelemetrySourceImpl.hpp"
#include <sstream>

bool FileTelemetrySourceImpl::openSource(){
    file.emplace(TELEMETRY_FILE_PATH);
    if(file->getFD() == -1){
        return false;
    }
    return true;
}

bool FileTelemetrySourceImpl::readSource(std::string& out){
    out.resize(128);
    int dataBytes = read(file->getFD(), out.data(), out.size());
    lseek(file->getFD(), 0, SEEK_SET);
    if(dataBytes <= 0){
        return false;
    }
    out.resize(dataBytes);

    return true;
}