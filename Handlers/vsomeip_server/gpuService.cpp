#include <CommonAPI/CommonAPI.hpp>
#include <v1/omnimetron/gpu/GpuUsageDataStubDefault.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace v1::omnimetron::gpu;

float randomGpuUsage(){
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<float> dist(0.0f, 100.0f);
    return dist(rng);
}

class GpuService: public GpuUsageDataStubDefault {
public:
    GpuService() = default;
    virtual ~GpuService() = default;
    virtual void requestGpuUsageData(const std::shared_ptr<CommonAPI::ClientId> _client, requestGpuUsageDataReply_t _reply) override {
        // Simulate GPU usage data retrieval
        float usage = randomGpuUsage();
        std::cout << "Received request for GPU usage data. Responding with: " << usage << "%" << std::endl;
        _reply(usage);
    }
};

int main(){
    // Registering the Service -> it reads commonapi.ini to know which binding to use(in my case vsomeip))
    auto runtime = CommonAPI::Runtime::get();
    // Creates your service instance on the heap wrapped in a shared_ptr 
    // CommonAPI requires shared ownership because the runtime holds a reference to it internally.
    auto service = std::make_shared<GpuService>();

    // Register the service with the runtime.
    bool success = runtime->registerService(
        "local",                          // domain
        "omnimetron.gpu.GpuUsageData",  // must match .fdepl InstanceId
        service                          // the service instance you created
    );

    if(!success){
        std::cerr << "Service registration failed!" << std::endl;
        return -1;
    }

    std::cout << "Service successfully registered." << std::endl;

    // Fire event every 3 seconds
    int counter = 0;
    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        // generate some random gpu usage data and fire the event
        float gpuUsage = randomGpuUsage();
        if(int(gpuUsage) > 80){
            std::cout << "Firing notifyGpuUsageDataChange event with value: " << gpuUsage << "%" << std::endl;
            service->fireNotifyGpuUsageDataChangeEvent(gpuUsage);
        }
    }

    return 0;
}