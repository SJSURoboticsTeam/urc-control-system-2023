#pragma once

#include <libhal/serial.hpp>
#include "ackermann_steering.hpp"

namespace sjsu::drive {
    /**
     * @brief Print wheel settings. Assumes tri wheel configuration
     * 
     * @param serial 
     * @param settings 
     */
    void print_wheel_settings(hal::serial& serial, std::span<wheel_setting> settings);
    /**
     * @brief Does a few tests on the ackermann steering math. Read serial output to confirm these are correct.
     * 
     * @param serial 
     * @param steering 
     */
    void test_steering_math(hal::serial& serial, ackermann_steering steering);
}  // namespace sjsu::drive
