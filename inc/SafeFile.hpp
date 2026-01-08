#pragma once
#include <fcntl.h>
#include <unistd.h>

class SafeFile{
    int fd{-1};
public:
    SafeFile(const char* path);
    ~SafeFile();
    // Delete Copy Constructor & Copy Assignment
    SafeFile(const SafeFile&) = delete;
    SafeFile& operator=(const SafeFile&) = delete;

    SafeFile(SafeFile&& other);
    SafeFile& operator=(SafeFile&& other);

    int getFD();
};