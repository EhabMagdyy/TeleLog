#pragma once
#include "LogManager.hpp"
#include "ILogSink.hpp"
#include <memory>

class LogManagerBuilder {
private:
    std::unique_ptr<LogManager> manager;

public:
    // Start with an empty LogManager
    LogManagerBuilder() : manager(std::make_unique<LogManager>()) {}
    // Add a sink to the LogManager
    LogManagerBuilder& addSink(std::unique_ptr<ILogSink> sink) {
        manager->addSink(std::move(sink));
        return *this; // allows chaining
    }
    // No adding for logs because it's is a runtime behavior, not part of constructing the LogManager
    // Build & return the constructed LogManager
    std::unique_ptr<LogManager> build() {
        return std::move(manager);
    }
};
