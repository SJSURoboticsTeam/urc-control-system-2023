#pragma once

#include <string_view>
#include <libhal-esp8266/at.hpp>
#include <span>

#include <libhal-util/serial.hpp>



namespace sjsu::drive {
    class tcp_client {
        public:
            virtual hal::status send(std::span<const hal::byte> bytes, hal::function_ref<hal::timeout_function> p_timeout) = 0;
            virtual hal::result<std::span<hal::byte>> recieve(std::span<hal::byte> bytes, hal::function_ref<hal::timeout_function> p_timeout) = 0;
            virtual hal::status check_connection(hal::function_ref<hal::timeout_function> p_timeout) = 0;
            virtual hal::status reestablish_connection(hal::function_ref<hal::timeout_function> p_timeout) = 0;

            inline bool is_connected() {
                return connection_status == connection_state::connection_established;
            }
            inline bool is_connected_to_router() {
                return connection_status  != connection_state::ap_disconnected;
            }

        private:
            enum class connection_state {
                ap_disconnected,
                set_ip_address,
                server_disconnected,
                connection_established,
            };

            connection_state connection_status = connection_state::ap_disconnected;
    };
};

