#pragma once

#include <string_view>
#include <libhal-esp8266/at.hpp>
#include <span>

#include <libhal-util/serial.hpp>

#include "../include/tcp_client.hpp"


namespace sjsu::drive {
    class esp_tcp_client : public tcp_client {
        public:
            hal::status send(std::span<const hal::byte> p_bytes, hal::function_ref<hal::timeout_function> p_timeout);

            hal::result<std::span<hal::byte>> recieve(std::span<hal::byte> p_bytes, hal::function_ref<hal::timeout_function> p_timeout);

            hal::status check_connection(hal::function_ref<hal::timeout_function> p_timeout);

            hal::status reestablish_connection(hal::function_ref<hal::timeout_function> p_timeout);

            static hal::result<esp_tcp_client> create(hal::esp8266::at& p_esp8266,
                    const std::string_view p_ssid,
                    const std::string_view p_password,
                    const hal::esp8266::at::socket_config& p_config,
                    const std::string_view p_ip);


            void use_debug_serial(hal::serial& p_debug);

        private:
            hal::serial* m_debug = nullptr;
            const std::string_view m_ssid;
            const std::string_view m_password;
            const hal::esp8266::at::socket_config& m_config;
            const std::string_view m_ip;

            hal::esp8266::at* m_esp8266;

            esp_tcp_client(hal::esp8266::at& p_esp8266,
                const std::string_view p_ssid,
                const std::string_view p_password,
                const hal::esp8266::at::socket_config& p_config,
                const std::string_view p_ip);


            enum class connection_state {
                ap_disconnected,
                set_ip_address,
                server_disconnected,
                connection_established,
            };

            connection_state connection_status = connection_state::ap_disconnected;

            void debug_print(std::string_view p_fixed_msg);
            void debug_print(int p_number);
    };
};