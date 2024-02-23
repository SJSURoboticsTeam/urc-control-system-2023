#pragma once

#include <libhal/serial.hpp>
#include "ackermann_steering.hpp"

namespace sjsu::drive {
    void print_wheel_settings(hal::serial& serial, std::span<wheel_setting> settings);
    void test_steering_math(hal::serial& serial, ackermann_steering steering);
}  // namespace sjsu::drive
