#pragma once

#include "ILogSink.hpp"
#include <fstream>

class FileSink : public ILogSink{
    std::ofstream file;
public:
    FileSink(const std::string& filePath, std::ios::openmode mode = std::ios::app); // append by default
    ~FileSink() = default;

    void write(const LogMessage& log);
};