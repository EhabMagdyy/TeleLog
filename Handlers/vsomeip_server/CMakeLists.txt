cmake_minimum_required(VERSION 3.10)
project(gpuService)

find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)
find_package(vsomeip3 REQUIRED)

file(GLOB ${PROJECT_NAME}_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src-gen/v1/omnimetron/gpu/*.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})

# Explicitly add include directories (fixes header not found errors)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src-gen
)

target_link_libraries(${PROJECT_NAME}
    vsomeip3
    CommonAPI
    CommonAPI-SomeIP
)