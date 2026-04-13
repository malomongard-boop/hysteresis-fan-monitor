cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hysteresis_fan_monitor VERSION 0.1.0 LANGUAGES CXX)

target_include_directories(app PRIVATE include)

target_sources(app PRIVATE
    src/main.cpp
    src/sensor_task.cpp
    src/regulator_task.cpp
    src/actuator_task.cpp
)