#include "source/SomeIPTelemetrySourceImpl.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>

bool SomeIPTelemetrySourceImpl::openSource() {
    auto runtime = CommonAPI::Runtime::get();

    proxy = runtime->buildProxy<v1::omnimetron::gpu::GpuUsageDataProxy>(
        "local", "omnimetron.gpu.GpuUsageData");

    if(!proxy) {
        std::cerr << "SomeIPTelemetrySourceImpl: failed to build proxy.\n";
        return false;
    }

    // Wait up to 5 s for the service to become available
    int retries = 50;
    while(!proxy->isAvailable() && retries-- > 0) {
        std::cout << "Waiting for gpuService..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if(!proxy->isAvailable()) {
        std::cerr << "SomeIPTelemetrySourceImpl: gpuService not available.\n";
        return false;
    }
    // Subscribe to events & Register callback to add logs on every incoming event
    proxy->getNotifyGpuUsageDataChangeEvent().subscribe(
        [this](const float gpuUsage) {
            if(!eventHandler) 
                return;
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << gpuUsage;
            eventHandler(oss.str());
        });

    std::cout << "SomeIPTelemetrySourceImpl: subscribed to GPU events.\n";
    return true;
}

bool SomeIPTelemetrySourceImpl::readSource(std::string& out) {
    if(!proxy || !proxy->isAvailable())
        return false;

    float usage = 0.0f;


    CommonAPI::CallStatus callStatus;
    proxy->requestGpuUsageData(callStatus, usage);
    if(callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "SomeIPTelemetrySourceImpl: call failed, status="
                    << static_cast<int>(callStatus) << "\n";
        return false;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << usage;
    out = oss.str();
    return true;
}