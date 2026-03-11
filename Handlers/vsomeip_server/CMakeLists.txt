cmake_minimum_required(VERSION 3.10)
project(gpuService)

find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)

file(GLOB ${PROJECT_NAME}_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src-gen/v1/omnimetron/gpu/*.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src-gen            
    ${CAPICXX_CORE_RUNTIME_INC}
    ${CAPICXX_SOMEIP_RUNTIME_INC}
    ${VSOMEIP_INC}
)

target_link_libraries(${PROJECT_NAME}
    vsomeip3
    CommonAPI
    CommonAPI-SomeIP
)