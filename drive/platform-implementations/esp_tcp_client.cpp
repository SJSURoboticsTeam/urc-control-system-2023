#include "../include/esp_tcp_client.hpp"

namespace sjsu::drive {
    hal::status esp_tcp_client::send(std::span<const hal::byte> p_bytes, hal::function_ref<hal::timeout_function> p_timeout) {
        if(connection_status != connection_state::connection_established) {
            HAL_CHECK(reestablish_connection(p_timeout));
        }
        auto status = m_esp8266->server_write(hal::as_bytes(p_bytes), p_timeout);
        if(!status) {
            debug_print("Send Failed\nAttempting to reconnect");
            HAL_CHECK(check_connection(p_timeout));
            HAL_CHECK(reestablish_connection(p_timeout));
            HAL_CHECK(send(p_bytes, p_timeout));
        }
        return hal::success();
    }

    hal::result<std::span<hal::byte>> esp_tcp_client::recieve(std::span<hal::byte> p_bytes, hal::function_ref<hal::timeout_function> p_timeout) {
        // if(connection_status != connection_state::connection_established) {
        //     HAL_CHECK(reestablish_connection(p_timeout));
        // }
        auto status = m_esp8266->server_read(p_bytes);

        // if(!status) {
        //     HAL_CHECK(check_connection(p_timeout));
        //     HAL_CHECK(reestablish_connection(p_timeout));
        //     return recieve(bytes, p_timeout);
        // }

        // return status.value().data;
        return HAL_CHECK(status).data;
    }

    hal::status esp_tcp_client::check_connection(hal::function_ref<hal::timeout_function> p_timeout) {
        if (!HAL_CHECK(m_esp8266->is_connected_to_server(p_timeout))) {
            if (!HAL_CHECK(m_esp8266->is_connected_to_ap(p_timeout))) {
                debug_print("We are disconnected from the router\n");
                connection_status = connection_state::ap_disconnected;
            }else {
                debug_print("We are disconnected from the server\n");
                connection_status = connection_state::server_disconnected;
            }
        }else {
            debug_print("We are connected to the server\n");
            connection_status = connection_state::connection_established;
        }
        return hal::success();
    }

    hal::status esp_tcp_client::reestablish_connection(hal::function_ref<hal::timeout_function> p_timeout) {
        switch(connection_status) {
            case connection_state::ap_disconnected:
                if (!HAL_CHECK(m_esp8266->is_connected_to_ap(p_timeout))) {
                    debug_print("Connecting to router...\n");
                    HAL_CHECK(m_esp8266->connect_to_ap(m_ssid, m_password, p_timeout));
                    connection_status = connection_state::set_ip_address;
                }
            case connection_state::set_ip_address:
                debug_print("Setting IP Address to \"");
                debug_print(m_ip);
                debug_print("\"...\n");
                HAL_CHECK(m_esp8266->set_ip_address(m_ip, p_timeout));
                connection_status = connection_state::server_disconnected;
            case connection_state::server_disconnected:
                debug_print("Connecting to server (");
                debug_print(m_config.domain);
                debug_print(":");
                debug_print(m_config.port);
                debug_print(")...\n");
                if (!HAL_CHECK(m_esp8266->is_connected_to_server(p_timeout))) {
                    HAL_CHECK(m_esp8266->connect_to_server(m_config, p_timeout));
                }
                connection_status = connection_state::connection_established;
            case connection_state::connection_established:
                break;
        };

        return hal::success();
    };

    hal::result<esp_tcp_client> esp_tcp_client::create(hal::esp8266::at& p_esp8266,
            const std::string_view p_ssid,
            const std::string_view p_password,
            const hal::esp8266::at::socket_config& p_config,
            const std::string_view p_ip) {
        esp_tcp_client client(p_esp8266, p_ssid, p_password, p_config, p_ip);
        return client;
    }


    void esp_tcp_client::use_debug_serial(hal::serial& p_debug) {
        m_debug = &p_debug;
    }

    esp_tcp_client::esp_tcp_client(hal::esp8266::at& p_esp8266,
        const std::string_view p_ssid,
        const std::string_view p_password,
        const hal::esp8266::at::socket_config& p_config,
        const std::string_view p_ip) : 
        m_ssid(p_ssid), 
        m_config(p_config),
        m_password(p_password),
        m_esp8266(&p_esp8266),
        m_ip(p_ip) {}

    void esp_tcp_client::debug_print(std::string_view p_fixed_msg) {
        if(m_debug != nullptr) {
            hal::print(*m_debug, p_fixed_msg);
        }
    }
    void esp_tcp_client::debug_print(int p_number) {
        if(m_debug != nullptr) {
            hal::print<16>(*m_debug, "%d", p_number);
        }
    }
}
