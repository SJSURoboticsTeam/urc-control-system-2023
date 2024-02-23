#include <libhal-util/serial.hpp>
#include "../include/drive_debug.hpp"

namespace sjsu::drive {
    void print_wheel_settings(hal::serial& serial, std::span<wheel_setting> settings) {
        hal::print<1024>(serial, "|FL: %-5.1f°, %-5.1f rpm | FR: %-5.1f°, %-5.1f rpm | B: %-5.1f°, %-5.1f rpm |\n", 
            settings[0].angle,
            settings[0].wheel_speed, 
            settings[1].angle,
            settings[1].wheel_speed, 
            settings[2].angle,
            settings[2].wheel_speed);
    }
    void test_steering_math(hal::serial& serial, ackermann_steering steering) {
        auto wheel_settings = steering.calculate_wheel_settings(std::numeric_limits<float>::infinity(), 20, 1);
        print_wheel_settings(serial, wheel_settings);

        wheel_settings = steering.calculate_wheel_settings(std::numeric_limits<float>::infinity(), -120, 1);
        print_wheel_settings(serial, wheel_settings);
        
        wheel_settings = steering.calculate_wheel_settings(0, 0, 1);
        print_wheel_settings(serial, wheel_settings);
        
        wheel_settings = steering.calculate_wheel_settings(1/ std::tan(60 * std::numbers::pi / 180), 0, 0.1);
        print_wheel_settings(serial, wheel_settings);
        
        wheel_settings = steering.calculate_wheel_settings(1/ std::tan(35.4 * std::numbers::pi / 180), 44.7, 0.5);
        print_wheel_settings(serial, wheel_settings);
    }
}