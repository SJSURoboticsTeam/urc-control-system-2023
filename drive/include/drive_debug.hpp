#pragma once

#include <libhal/serial.hpp>
#include "ackermann_steering.hpp"

namespace sjsu::drive {
    /**
     * @brief Print wheel settings. Assumes the tri-wheel configuration
     * 
     * @param console Console to print
     * @param settings The wheel settings calculated for each leg of the tri-wheel
     */
    void print_wheel_settings(hal::serial& console, std::span<wheel_setting> settings);
    /**
     * @brief Does a few tests on the ackermann steering math. Read serial output to confirm these are correct.
     * 
     * @param console Console to print
     * @param steering The steering manager to test.
     */
    void test_steering_math(hal::serial& console, ackermann_steering steering);
}  // namespace sjsu::drive
