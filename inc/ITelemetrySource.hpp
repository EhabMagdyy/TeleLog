#pragma once
#include <string>

class ITelemetrySource{
public:
    virtual bool openSource() = 0;
    virtual bool readSource(std::string& out) = 0;
    virtual ~ITelemetrySource() = default;
};