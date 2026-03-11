#pragma once
#include "ITelemetrySource.hpp"
#include <CommonAPI/CommonAPI.hpp>
#include <v1/omnimetron/gpu/GpuUsageDataProxy.hpp>
#include <memory>
#include <mutex>
#include <atomic>

class SomeIPTelemetrySourceImpl : public ITelemetrySource {
    std::shared_ptr<v1::omnimetron::gpu::GpuUsageDataProxy<>> proxy;

    // Latest GPU usage pushed by the service event; guarded by mutex
    mutable std::mutex      cacheMtx;
    float                   cachedUsage { 0.0f };
    std::atomic<bool>       hasEvent    { false };  // true once ≥1 event received

public:
    SomeIPTelemetrySourceImpl() = default;
    virtual bool openSource() override;
    virtual bool readSource(std::string& out) override;
};