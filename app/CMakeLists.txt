add_executable(${PROJECT_NAME} main.cpp)

# --whole-archive forces the linker to include every .o from TeleLogLib, even
# those with no directly-referenced symbols.  This is required so that the
# INITIALIZER(registerGpuUsageDataSomeIPProxy) GCC constructor in the generated
# GpuUsageDataSomeIPProxy.cpp runs at start-up and registers the CommonAPI-SomeIP
# proxy factory.  Without it, the static linker silently drops that translation
# unit and buildProxy() returns nullptr.
target_link_libraries(${PROJECT_NAME} PRIVATE
    -Wl,--whole-archive TeleLogLib -Wl,--no-whole-archive
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/inc/sink)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/inc/source)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/inc/log)