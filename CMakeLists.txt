cmake_minimum_required(VERSION 3.29)
project(Door_Locker_Security_System C)

set(CMAKE_C_STANDARD 11)

include_directories(Common/HAL)
include_directories(Common/MCAL)
include_directories(HMI_ECU)

add_executable(Door_Locker_Security_System
    Common/HAL/comm_interface.c
    Common/HAL/comm_interface.h
    Common/MCAL/tm4c123gh6pm.h
    Common/MCAL/uart.c
    Common/MCAL/uart.h
        Control_ECU/Tests/Eeprom/main.c
        Control_ECU/Drivers/Eeprom/eeprom.c
        Control_ECU/Tests/Eeprom/mock_eeprom_hw.c
        Control_ECU/Tests/Eeprom/eeprom_unit_test.c
        External/unity.c)
