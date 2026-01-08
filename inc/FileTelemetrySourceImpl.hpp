#pragma once
#include "ITelemetrySource.hpp"
#include "SafeFile.hpp"

class FileTelemetrySourceImpl : public ITelemetrySource {
    SafeFile file;  // My RAII wrapper for file descriptor
public:
    virtual bool openSource() override;
    virtual bool readSource(std::string& out) override;
};