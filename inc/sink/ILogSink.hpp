#pragma once
#include "LogMessage.hpp"

class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void write(const LogMessage& log) = 0;
};