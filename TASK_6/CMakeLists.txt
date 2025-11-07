cmake_minimum_required(VERSION 3.20)
project(task6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR})

#添加线程支持
find_package(Threads REQUIRED)

add_executable(new_task6 ${PROJECT_SOURCE_DIR}/new_task6.cpp)

#链接线程库
target_link_libraries(task6 Threads::Threads)
