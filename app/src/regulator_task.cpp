#include <zephyr/kernel.h>
#include "tasks.hpp"

K_MSGQ_DEFINE(cmd_queue, sizeof(RegulatorMessage), CMD_QUEUE_DEPTH, 4); // Message queue to actuator task

extern struct k_msgq temp_queue; // Message queue from sensor task

static int32_t s_min_threshold {65};
static int32_t s_max_threshold {75};
static k_mutex s_threshold_mutex {};

// Thread-safe API to update the regulator thresholds at runtime
void regulator_set_thresholds(int32_t min, int32_t max)
{
    k_mutex_lock(&s_threshold_mutex, K_FOREVER);
    s_min_threshold = min;
    s_max_threshold = max;
    k_mutex_unlock(&s_threshold_mutex);
}

static FanState s_current_fan_state {FAN_OFF};

void regulator_task(void*, void*, void*)
{
    k_mutex_init(&s_threshold_mutex);
    
    printk("[REGULATOR] Task started\n");

    SensorMessage msgFromSensor {};
    RegulatorMessage msgToActuator {};
    int32_t min_threshold {};
    int32_t max_threshold {};

    while (true)
    {
        if(k_msgq_get(&temp_queue, &msgFromSensor, K_FOREVER) == 0)
        {
            // Read thresholds under mutex to prevent partial reads
            k_mutex_lock(&s_threshold_mutex, K_FOREVER);
            min_threshold = s_min_threshold;
            max_threshold = s_max_threshold;
            k_mutex_unlock(&s_threshold_mutex);

            // Hysteresis control logic
            if (msgFromSensor.value >= max_threshold && s_current_fan_state == FAN_OFF)
            {
                s_current_fan_state = FAN_ON;
                msgToActuator.fan_command = FAN_ON;
            }
            else if (msgFromSensor.value <= min_threshold && s_current_fan_state == FAN_ON)
            {
                s_current_fan_state = FAN_OFF;
                msgToActuator.fan_command = FAN_OFF;
            }
            else
            {
                // No change, skip sending command
                continue;
            }

            msgToActuator.timestamp = msgFromSensor.timestamp; // Use sensor timestamp for better correlation

            // Send the command to the actuator task, with a timeout to avoid blocking indefinitely
            if (k_msgq_put(&cmd_queue, &msgToActuator, K_MSEC(10)) != 0)
            {
                printk("[REGULATOR] Warning: cmd_queue full, command dropped\n");
            }
        }
    }
}

K_THREAD_DEFINE(
    regulator_tid,
    REGULATOR_STACK_SIZE,
    regulator_task,
    NULL, NULL, NULL,
    REGULATOR_PRIORITY,
    0,
    0
);