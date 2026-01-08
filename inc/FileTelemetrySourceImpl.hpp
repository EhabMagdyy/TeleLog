#pragma once
#include "ITelemetrySource.hpp"
#include "SafeFile.hpp"
#include <optional> // C++17 - allows for delayed initialization

constexpr const char* TELEMETRY_FILE_PATH = "/home/ehab/Documents/ITI_9Months/CppProject/file.txt";

class FileTelemetrySourceImpl : public ITelemetrySource {
    std::optional<SafeFile> file;  // My RAII wrapper for file descriptor
public:
    FileTelemetrySourceImpl() = default;
    virtual bool openSource() override;
    virtual bool readSource(std::string& out) override;
};