#pragma once
#include "ITelemetrySource.hpp"
#include "SafeSocket.hpp"
#include <optional> // C++17 - allows for delayed initialization

constexpr const char* TELEMETRY_SOCKET_PATH = "/home/ehab/Documents/ITI_9Months/CppProject/sokt.sock";

class SocketTelemetrySourceImpl : public ITelemetrySource {
    std::optional<SafeSocket> socket;  // My RAII wrapper for socket descriptor
public:
    SocketTelemetrySourceImpl() = default;
    virtual bool openSource() override;
    virtual bool readSource(std::string& out) override;
};