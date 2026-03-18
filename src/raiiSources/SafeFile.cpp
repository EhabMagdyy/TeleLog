#include "SafeFile.hpp"

SafeFile::SafeFile(const char* path){
    fd = open(path, O_RDONLY);
}
SafeFile::~SafeFile(){
    if(fd != -1){
        close(fd);
    }
}

SafeFile::SafeFile(SafeFile&& other) noexcept : fd(other.fd){
    other.fd = -1;
}

SafeFile& SafeFile::operator=(SafeFile&& other) noexcept {
    if(this != &other){
        if(fd != -1){
            close(fd);
        }
        fd = other.fd;
        other.fd = -1;
    }
    return *this;
}

int SafeFile::getFD(){
    return fd;
}