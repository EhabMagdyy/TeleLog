cmake_minimum_required(VERSION 3.10)
project(LogTelemetrySystem)

add_executable(
    ${PROJECT_NAME}
    src/LogMessage.cpp
    app/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE inc/)