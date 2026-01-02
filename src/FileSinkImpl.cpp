#include "FileSinkImpl.hpp"
#include <iostream>

FileSink::FileSink(const std::string& filePath, std::ios::openmode mode){
    file = std::ofstream(filePath, mode);
    if(!file.is_open()){
        std::cout << "Failed to open file: " << filePath << std::endl;
    }
}

void FileSink::write(const LogMessage& log){
    file << log;
}