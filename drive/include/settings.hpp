#pragma once

#include <libhal/can.hpp>
#include <libhal/serial.hpp>
#include <libhal-esp8266/at.hpp>
#include <libhal-util/units.hpp>


namespace sjsu::drive {
    namespace settings {
        using namespace hal::literals;
        using namespace std::chrono_literals;

        constexpr int terminal_uart = 0;
        constexpr size_t terminal_uart_buffer_size = 1024;
        constexpr hal::serial::settings terminal_uart_settings = {
            .baud_rate = 38400
        };
        
        constexpr int can_bus = 2;
        constexpr hal::can::settings can_settings = {
            .baud_rate = 1.0_MHz, 
        };

        hal::rpm max_steer_speed = 5.0_rpm;
        hal::rpm max_propulsion_speed = 100.0_rpm;

        struct pin {
            std::uint8_t port;
            std::uint8_t pin;
        };
        struct leg_settings {
            std::uint32_t steering_motor_id;
            std::uint32_t propulsion_motor_id;
            pin hall;
        };
        
        constexpr leg_settings left_leg{
            .steering_motor_id = 0x141,
            .propulsion_motor_id = 0x142,
            .hall{
                .port = 1,
                .pin = 22
            }
        };
        constexpr leg_settings right_leg{
            .steering_motor_id = 0x143,
            .propulsion_motor_id = 0x144,
            .hall{
                .port = 1,
                .pin = 15
            }
        };
        constexpr leg_settings back_leg{
            .steering_motor_id = 0x145,
            .propulsion_motor_id = 0x146,
            .hall{
                .port = 1,
                .pin = 23
            }
        };

    }

};