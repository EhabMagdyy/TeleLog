#pragma once

#include "ILogSink.hpp"
#include <iostream>

class ConsoleSink : public ILogSink{
public:
    ConsoleSink() = default;
    ~ConsoleSink() = default;

    void write(const LogMessage& log);
};