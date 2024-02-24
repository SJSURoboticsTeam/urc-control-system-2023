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

        constexpr std::uint8_t terminal_uart = 0;
        constexpr std::size_t terminal_uart_buffer_size = 1024;
        constexpr hal::serial::settings terminal_uart_settings = {
            .baud_rate = 38400
        };
        
        constexpr int can_bus = 2;
        constexpr hal::can::settings can_settings = {
            .baud_rate = 1.0_MHz, 
        };

        constexpr hal::rpm max_steer_speed = 5.0_rpm;
        constexpr hal::rpm max_propulsion_speed = 100.0_rpm;

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

        constexpr int max_drive_angle = 100; // degrees?
        constexpr int max_speed = 100; // percent
        constexpr float speed_lerp_speed = 0.1f; // LMAO this is just how fast the speed is lerped.
        constexpr int mission_control_request_period = 10;

        constexpr std::string_view ssid = "TP-Link_FC30";
        constexpr std::string_view password = "R0Bot1cs3250";

        constexpr std::string_view drive_ip = "192.168.0.224";

        constexpr std::string_view server_ip = "192.168.0.211";
        constexpr std::uint16_t server_port = 5000;

        
        constexpr std::string_view get_request = "GET /drive HTTP/1.1\r\n"
                                                  "Host: 192.168.0.211:5000\r\n"
                                                  "\r\n";

        constexpr std::size_t http_buffer_size = 1024;
        constexpr hal::esp8266::at::socket_config client_config {
            .type = hal::esp8266::at::socket_type::tcp,
            .domain = server_ip, // Server IP
            .port = server_port,
        };

        
        constexpr std::uint8_t esp_uart = 1;
        constexpr std::size_t esp_uart_buffer_size = 8192;
        constexpr hal::serial::settings esp_uart_settings = {
            .baud_rate = 115200
        };
        
        
        constexpr float left_leg_drive_offset = 37;      // 41
        constexpr float right_leg_drive_offset = 199.5;  // 200
        constexpr float back_leg_drive_offset = 117;     // 122

        constexpr float left_leg_spin_offset = 242;
        constexpr float right_leg_spin_offset = 0;
        constexpr float back_leg_spin_offset = 0;
    }

};