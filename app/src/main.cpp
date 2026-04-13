#include <zephyr/kernel.h>
#include <zephyr/version.h>

int main()
{
    printk("=== Hysteresis Fan Monitor ===\n");
    printk("Zephyr version: %s\n", KERNEL_VERSION_STRING);
    return 0;
}