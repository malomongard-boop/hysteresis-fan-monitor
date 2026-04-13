#include "tasks.hpp"
#include <zephyr/kernel.h>

K_MSGQ_DEFINE(temp_queue, sizeof(SensorMessage), TEMP_QUEUE_DEPTH, 4); // Message queue to regulator task
K_SEM_DEFINE(sensor_sem, 0, 1); // Semaphore signaled by the timer ISR to trigger acquisition

// Temperature fluctuations simulation
static int32_t s_temperature {20};
static int32_t s_cycle {0};

// Timer callback — runs in ISR context
// Only signals the semaphore; actual work is done in the task
static void sensor_callback([[maybe_unused]]struct k_timer *timer)
{
    k_sem_give(&sensor_sem);
}

K_TIMER_DEFINE(sensor_timer, sensor_callback, NULL); // Timer for periodic acquisition

// Task that simulate the temperature changes and send raw data to the regulator task
void sensor_task(void*, void*, void*)
{
    printk("[SENSOR] Task started, period=%dms\n", ACQUISITION_PERIOD_MS);
    k_timer_start(&sensor_timer,
                  K_MSEC(ACQUISITION_PERIOD_MS),
                  K_MSEC(ACQUISITION_PERIOD_MS));

    while (s_cycle < 20)
    {
        k_sem_take(&sensor_sem, K_FOREVER);
        SensorMessage msg {};
        msg.value = s_temperature;
        msg.timestamp = k_uptime_get();

        //printk("[SENSOR] Acquired value=%d at %lldms\n", msg.value, msg.timestamp);

        // Simulate temperature changes with a simple pattern: increase until 90, then reset to 20
        if (s_temperature >= 90)
        {
            s_temperature = 20; // reset to minimum
            s_cycle++;
        }
        else
        {
            s_temperature++; // increase temperature
        }

        // Send the message to the regulator task, with a timeout to avoid blocking indefinitely
        if (k_msgq_put(&temp_queue, &msg, K_MSEC(10)) != 0)
        {
            printk("[SENSOR] Warning: queue full, message dropped\n");
        }
    }
    k_timer_stop(&sensor_timer);
}

// Static thread declaration — stack and TCB allocated at compile time
K_THREAD_DEFINE(
    sensor_tid, // Thread ID
    SENSOR_STACK_SIZE, // Stack size
    sensor_task, // Thread entry function
    NULL, NULL, NULL, // No arguments
    SENSOR_PRIORITY, // Thread priority
    0, // Options
    0 // Delay (start immediately)
);