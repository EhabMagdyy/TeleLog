add_library(TeleLogLib
    log/LogManager.cpp
    sink/ConsoleSinkImpl.cpp
    sink/FileSinkImpl.cpp
    log/LogMessage.cpp
    SafeFile.cpp
    source/FileTelemetrySourceImpl.cpp
    SafeSocket.cpp
    source/SocketTelemetrySourceImpl.cpp
    ThreadPool.cpp
)

target_include_directories(TeleLogLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
target_include_directories(TeleLogLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../inc/sink)
target_include_directories(TeleLogLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../inc/source)
target_include_directories(TeleLogLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../inc/log)