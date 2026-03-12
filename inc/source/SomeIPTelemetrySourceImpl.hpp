#pragma once
#include "ITelemetrySource.hpp"
#include <CommonAPI/CommonAPI.hpp>
#include <v1/omnimetron/gpu/GpuUsageDataProxy.hpp>
#include <functional>

// Singleton for SOME/IP  & Adapter for telemetry source interface
class SomeIPTelemetrySourceImpl : public ITelemetrySource {
    std::shared_ptr<v1::omnimetron::gpu::GpuUsageDataProxy<>> proxy;
    std::function<void(std::string)> eventHandler;

    explicit SomeIPTelemetrySourceImpl(std::function<void(std::string)> handler)
        : eventHandler(std::move(handler)) {}

public:
    static SomeIPTelemetrySourceImpl& getInstance(std::function<void(std::string)> handler = {}){
        static SomeIPTelemetrySourceImpl instance(std::move(handler)); // created once > call constructor only once
        return instance;    // returns same instance on every call after its first creation(C++11 guarantees thread-safe)
    }

    SomeIPTelemetrySourceImpl(const SomeIPTelemetrySourceImpl&) = delete;
    SomeIPTelemetrySourceImpl& operator=(const SomeIPTelemetrySourceImpl&) = delete;
    SomeIPTelemetrySourceImpl(SomeIPTelemetrySourceImpl&&) = delete;
    SomeIPTelemetrySourceImpl& operator=(SomeIPTelemetrySourceImpl&&) = delete;

    virtual bool openSource() override;
    virtual bool readSource(std::string& out) override;
};