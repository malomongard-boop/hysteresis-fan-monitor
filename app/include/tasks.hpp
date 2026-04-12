#pragma once

#include <zephyr/kernel.h>

struct SensorMessage 
{
    int32_t value {}; // simulated raw temperature value
    int64_t timestamp {}; // acquisition timestamp (ms)
};

enum FanState
{
    FAN_OFF,
    FAN_ON
};
struct RegulatorMessage 
{
    FanState fan_command {}; // command to turn the fan on or off
    int64_t timestamp {}; // command timestamp (ms)
};

// Stak sizes
constexpr int SENSOR_STACK_SIZE {2048};
constexpr int REGULATOR_STACK_SIZE {2048};
constexpr int ACTUATOR_STACK_SIZE {2048};

// Task priorities (lower value = higher priority)
constexpr int SENSOR_PRIORITY {5};
constexpr int REGULATOR_PRIORITY {6};
constexpr int ACTUATOR_PRIORITY {7};

// Message queue depths
constexpr int TEMP_QUEUE_DEPTH {10};
constexpr int CMD_QUEUE_DEPTH {10};

// Acquisition period in milliseconds
constexpr int ACQUISITION_PERIOD_MS {200};