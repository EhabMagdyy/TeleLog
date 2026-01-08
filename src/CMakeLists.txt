add_library(TeleLogLib
    LogManager.cpp
    ConsoleSinkImpl.cpp
    FileSinkImpl.cpp
    LogMessage.cpp
    SafeFile.cpp
    FileTelemetrySourceImpl.cpp
)

target_include_directories(TeleLogLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../inc)