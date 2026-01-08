#include "FileTelemetrySourceImpl.hpp"

bool FileTelemetrySourceImpl::openSource(){
    file.emplace("source.txt"); // Initialize the optional with SafeFile
    if(file->getFD() == -1){
        return false;
    }
    return true;
}

bool FileTelemetrySourceImpl::readSource(std::string& out){
    int noOfCharsRead = 0;
    out.resize(10);
    // out.data() is a pointer to internal data. It is undefined to modify the contents through the returned pointer.
    // To get a pointer that allows modifying the contents use &str[0] instead, (or in C++17 the non-const str.data() overload).
    noOfCharsRead = read(file->getFD(), out.data(), 10);    // C++17
    if(noOfCharsRead <= 0){
        return false;
    }
    out.resize(noOfCharsRead);
    return true;
}