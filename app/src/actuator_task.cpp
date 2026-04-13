#include <zephyr/kernel.h>
#include "tasks.hpp"

extern struct k_msgq cmd_queue; // Message queue from regulator task
static int onOff_counter {0};

void actuator_task(void*, void*, void*)
{
    printk("[ACTUATOR] Task started\n");

    RegulatorMessage msgFromRegulator {};

    while (true)
    {
        if (k_msgq_get(&cmd_queue, &msgFromRegulator, K_FOREVER) == 0)
        {
            if (msgFromRegulator.fan_command == FAN_ON)
            {
                onOff_counter++;
                printk("[ACTUATOR] Received command to turn FAN ON at %lldms\n", msgFromRegulator.timestamp);
            }
            else
            {
                printk("[ACTUATOR] Received command to turn FAN OFF at %lldms\n", msgFromRegulator.timestamp);
            }

            if (onOff_counter % 10 == 0 && msgFromRegulator.fan_command == FAN_ON) // Print status every 10 commands
            {
                printk("[ACTUATOR] Fan ON/OFF counter: %d\n", onOff_counter);
            }
        }
    }
}

K_THREAD_DEFINE(
    actuator_tid, // Thread ID
    ACTUATOR_STACK_SIZE, // Stack size
    actuator_task, // Thread entry function
    NULL, NULL, NULL, // No arguments
    ACTUATOR_PRIORITY, // Thread priority
    0, // Options
    0 // Delay (start immediately)
);