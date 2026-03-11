#pragma once
#include "ITelemetrySource.hpp"
#include <CommonAPI/CommonAPI.hpp>
#include <v1/omnimetron/gpu/GpuUsageDataProxy.hpp>
#include <memory>

class SomeIPTelemetrySourceImpl : public ITelemetrySource {
    // Store the fully instantiated type, but don't alias it
    std::shared_ptr<v1::omnimetron::gpu::GpuUsageDataProxy<>> proxy;
public:
    SomeIPTelemetrySourceImpl() = default;
    virtual bool openSource() override;
    virtual bool readSource(std::string& out) override;
};