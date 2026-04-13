# hysteresis-fan-monitor
This is a personal project to learn some basics of ZephyrOS with C++.
It's a simulation of a fan regulator that receives data from a temperature sensor.

## Overview
A regulator controls a fan with ON/OFF command.
It is based on an hysteresis:
- T < 65°C: Turn off the fan.
- T > 75°C: Turn on the fan.
- 65°C < T < 75°C: Nothing to do.

The sensor is simulate with a cyclic sawtooth from 20°C to 90°C and an acquisition period of 200ms.

## Architecture
The architecture of the project is as follow: 
```text
sensor -> [temp_queue] -> regulator -> [cmd_queue] -> actuator
    |                          |                         |
    |                          |                         |
  simulate T°C              k_mutex               display information
                           hysteresis
```

A sensor task simulates temperature values from 20°C to 90°C with a period of 200ms using a k_timer for precision and consistency. The k_timer only add a ressource to a semaphore to unlock the sensor task, it is done in a ISR context.

The sensor task send data to the regulator task through a message queue.
The regulator task operate the hysteresis to command the fan position (ON/OFF).
An API is available to change the threshold (min/max) protected by a mutex.

The regulator task send information of the command to the actuator through a message queue.
The actuator displays when on or off is done and the number of on/off cycle done every 10 cycle.

The project files are organized like this:
```text
/app
    include/
        tasks.hpp --> global constants
    src/
        actuator_task.cpp
        main.cpp
        regulator_task.cpp
        sensor_task.cpp
    CMakeLists.txt --> build configuration
    prj.conf --> Zephyr kernel configuration
```

## Pre-requisites
- Zephyr version: 3.7.0
- SDK Zephyr: 0.16.8
- west version: 1.5.0
- CMake version: 3.28.3
- Target: qemu_x86 (simulator)
- Language: C++20
- Python version: 3.12.3

## Build & launch
To build your project, you'll need to install all the pre-requisites, see Zephyr official documentation here: https://docs.zephyrproject.org/latest/index.html
Then run the following commands:
```bash
cd ~/zephyrproject/zephyr
rm -rf ~/zephyrproject/zephyr/build
west build -b qemu_x86 ~/hysteresis-fan-monitor/app
```
Where ```~/zephyrproject/``` is your workspace and ```~/hysteresis-fan-monitor/``` is the clone of the github repository.

To test your build on the qemu_x86 simulator, use the following command:
```bash
west build -t run
```
## Expected output
When running your build, you must have something like this:
```
-- west build: running target run
[0/1] To exit from QEMU enter: 'CTRL+a, x'[QEMU] CPU: qemu32,+nx,+pae
SeaBIOS (version zephyr-v1.0.0-0-g31d4e0e-dirty-20200714_234759-fv-az50-zephyr)
Booting from ROM..
*** Booting Zephyr OS build v3.7.0 ***
=== Hysteresis Fan Monitor ===
Zephyr version: 3.7.0
[SENSOR] Task started, period=200ms
[REGULATOR] Task started
[ACTUATOR] Task started
[ACTUATOR] Received command to turn FAN ON at 11210ms
[ACTUATOR] Received command to turn FAN OFF at 14410ms
[ACTUATOR] Received command to turn FAN ON at 25410ms
[ACTUATOR] Received command to turn FAN OFF at 28610ms
[ACTUATOR] Received command to turn FAN ON at 39610ms
[ACTUATOR] Received command to turn FAN OFF at 42810ms
[ACTUATOR] Received command to turn FAN ON at 53810ms
[ACTUATOR] Received command to turn FAN OFF at 57010ms
[ACTUATOR] Received command to turn FAN ON at 68010ms
[ACTUATOR] Received command to turn FAN OFF at 71210ms
[ACTUATOR] Received command to turn FAN ON at 82210ms
[ACTUATOR] Received command to turn FAN OFF at 85410ms
[ACTUATOR] Received command to turn FAN ON at 96410ms
[ACTUATOR] Received command to turn FAN OFF at 99610ms
[ACTUATOR] Received command to turn FAN ON at 110610ms
[ACTUATOR] Received command to turn FAN OFF at 113810ms
[ACTUATOR] Received command to turn FAN ON at 124810ms
[ACTUATOR] Received command to turn FAN OFF at 128010ms
[ACTUATOR] Received command to turn FAN ON at 139010ms
[ACTUATOR] Fan ON/OFF counter: 10
[ACTUATOR] Received command to turn FAN OFF at 142210ms
[ACTUATOR] Received command to turn FAN ON at 153210ms
[ACTUATOR] Received command to turn FAN OFF at 156410ms
```

## License

MIT (see related file).
