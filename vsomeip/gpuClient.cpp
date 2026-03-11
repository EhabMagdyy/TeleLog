#include <CommonAPI/CommonAPI.hpp>
#include <v1/omnimetron/gpu/GpuUsageDataProxy.hpp>
#include <iostream>
#include <thread>
#include <chrono>

int main(){
    // Building the Proxy to call the service
    auto runtime = CommonAPI::Runtime::get();
    // "local" and "GettingStarted.Instance" must match the domain and instanceId used in the service registration
    auto proxy = runtime->buildProxy<v1::omnimetron::gpu::GpuUsageDataProxy>("local", "omnimetron.gpu.GpuUsageData");

    // Wait until the service is available before making calls
    if (!proxy->isAvailable()) {
        std::cout << "Waiting for service..." << std::endl;
        while (!proxy->isAvailable())
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Service found." << std::endl;
    }

    
    CommonAPI::CallStatus callStatus;
    float gpuUsage;
    // Call the sayHello method on the service. The parameters are:
    // - callStatus: an output parameter that will contain the status of the call after it returns
    // - result: an output parameter that will contain the response from the service (the "message" field in the .fidl)
    proxy->requestGpuUsageData(callStatus, gpuUsage);

    // Check call succeeded
    if(callStatus == CommonAPI::CallStatus::SUCCESS){
        std::cout << "Response: " << gpuUsage << std::endl;
    } 
    else{
        std::cout << "Call failed with status: " << static_cast<int>(callStatus) << std::endl;
    }

    // Subscribe to event
    proxy->getNotifyGpuUsageDataChangeEvent().subscribe([](const float gpuUsage) {
        std::cout << "Event received: " << gpuUsage << std::endl;
    });

    // Keep client alive — events arrive on background thread
    std::cout << "Listening for events..." << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}