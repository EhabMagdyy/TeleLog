#pragma once
#include "ITelemetrySource.hpp"
#include "SafeFile.hpp"
#include <optional> // C++17 - allows for delayed initialization

class FileTelemetrySourceImpl : public ITelemetrySource {
    std::optional<SafeFile> file;  // My RAII wrapper for file descriptor
public:
    FileTelemetrySourceImpl() = default;
    virtual bool openSource() override;
    virtual bool readSource(std::string& out) override;
};