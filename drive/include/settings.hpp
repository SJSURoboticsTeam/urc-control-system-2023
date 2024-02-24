#pragma once

#include <libhal/can.hpp>
#include <libhal/serial.hpp>
#include <libhal-esp8266/at.hpp>
#include <libhal-util/units.hpp>

#include <string_view>

namespace sjsu::drive {
    namespace settings {
        using namespace hal::literals;
        using namespace std::chrono_literals;

        // Terminal/Console uart settings.
        constexpr std::uint8_t terminal_uart = 0; // platforms/lpc4078.cpp
        constexpr std::size_t terminal_uart_buffer_size = 1024;  // platforms/lpc4078.cpp
        constexpr hal::serial::settings terminal_uart_settings = {
            .baud_rate = 38400 // bytes/s
        }; // platforms/lpc4078.cpp

        // CAN settings
        constexpr std::uint8_t can_bus = 2;  // platforms/lpc4078.cpp
        constexpr hal::can::settings can_settings = {
            .baud_rate = 1.0_MHz, 
        };  // platforms/lpc4078.cpp

        // This is how fast the motors can steer
        constexpr hal::rpm max_steer_speed = 5.0_rpm; // platforms/lpc4078.cpp
        // This is how fast the rover can move
        constexpr hal::rpm max_propulsion_speed = 100.0_rpm; // platforms/lpc4078.cpp

        struct pin {
            std::uint8_t port;
            std::uint8_t pin;
        };
        struct leg_settings {
            std::uint32_t steering_motor_id; // CAN ID
            std::uint32_t propulsion_motor_id; // CAN ID
            pin hall; // HALL PIN
        };

        // Motor and hall effect configuration for each leg.
        
        constexpr leg_settings left_leg{
            .steering_motor_id = 0x141,
            .propulsion_motor_id = 0x142,
            .hall{
                .port = 1,
                .pin = 22
            }
        }; // platforms/lpc4078.cpp
        constexpr leg_settings right_leg{
            .steering_motor_id = 0x143,
            .propulsion_motor_id = 0x144,
            .hall{
                .port = 1,
                .pin = 15
            }
        }; // platforms/lpc4078.cpp
        constexpr leg_settings back_leg{
            .steering_motor_id = 0x145,
            .propulsion_motor_id = 0x146,
            .hall{
                .port = 1,
                .pin = 23
            }
        }; // platforms/lpc4078.cpp

        // Hard speed and drive limits.
        // speed is a percent.
        constexpr int max_drive_angle = 12; // degrees?  implementations/rules_engine.hpp
        constexpr int max_speed = 100; // percent  implementations/rules_engine.hpp

        // LMAO what a funky name. this is just how fast the speed is lerped.
        constexpr float speed_lerp_speed = 0.1f; // include/command_lerper.hpp
        
        // This controls how often to check mission_control
        constexpr int mission_control_request_period = 10; // loops per check    applcations/application.cpp

        // Router ssid/password
        constexpr std::string_view ssid = "TP-Link_FC30"; // platforms/lpc4078.cpp
        constexpr std::string_view password = "R0Bot1cs3250"; // platforms/lpc4078.cpp

        // This device's ip
        constexpr std::string_view drive_ip = "192.168.0.224"; // platforms/lpc4078.cpp

        // Server ip/port
        constexpr std::string_view server_ip = "192.168.0.211"; // platforms/lpc4078.cpp
        constexpr std::uint16_t server_port = 5000; // platforms/lpc4078.cpp

        // GET request for mission control.
        constexpr std::string_view get_request = "GET /drive HTTP/1.1\r\n"
                                                  "Host: 192.168.0.211:5000\r\n"
                                                  "\r\n"; // platforms/lpc4078.cpp
        // MC HTTP settings
        constexpr std::size_t http_buffer_size = 1024; // platforms/lpc4078.cpp
        constexpr hal::esp8266::at::socket_config client_config {
            .type = hal::esp8266::at::socket_type::tcp,
            .domain = server_ip, // Server IP
            .port = server_port,
        }; // platforms/lpc4078.cpp

        // AT ESP UART Settings
        constexpr std::uint8_t esp_uart = 1; // platforms/lpc4078.cpp
        constexpr std::size_t esp_uart_buffer_size = 8192; // platforms/lpc4078.cpp
        constexpr hal::serial::settings esp_uart_settings = {
            .baud_rate = 115200 // bytes / s
        }; // platforms/lpc4078.cpp
        

        // VERY MAGIC NUMBERS
        constexpr float left_leg_drive_offset = 37;      // 41        implementations/steer_modes.hpp
        constexpr float right_leg_drive_offset = 199.5;  // 200       implementations/steer_modes.hpp
        constexpr float back_leg_drive_offset = 117;     // 122       implementations/steer_modes.hpp

        constexpr float left_leg_spin_offset = 242; // implementations/steer_modes.hpp
        constexpr float right_leg_spin_offset = 0; // implementations/steer_modes.hpp
        constexpr float back_leg_spin_offset = 0; // implementations/steer_modes.hpp
    }

};