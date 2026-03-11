find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)

add_library(TeleLogLib
    log/LogManager.cpp
    sink/ConsoleSinkImpl.cpp
    sink/FileSinkImpl.cpp
    log/LogMessage.cpp
    SafeFile.cpp
    source/FileTelemetrySourceImpl.cpp
    SafeSocket.cpp
    source/SocketTelemetrySourceImpl.cpp
    source/SomeIPTelemetrySourceImpl.cpp
    ThreadPool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src-gen/v1/omnimetron/gpu/GpuUsageDataSomeIPProxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src-gen/v1/omnimetron/gpu/GpuUsageDataSomeIPDeployment.cpp
)

target_include_directories(TeleLogLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/sink
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/source
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/log
    ${CMAKE_CURRENT_SOURCE_DIR}/../src-gen
    ${CAPICXX_CORE_RUNTIME_INC}
    ${CAPICXX_SOMEIP_RUNTIME_INC}
    ${VSOMEIP_INC}
)

target_link_libraries(TeleLogLib PUBLIC
    vsomeip3
    vsomeip3-cfg
    vsomeip3-e2e
    vsomeip3-sd
    CommonAPI
    CommonAPI-SomeIP
)