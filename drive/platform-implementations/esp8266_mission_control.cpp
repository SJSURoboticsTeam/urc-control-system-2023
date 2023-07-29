#pragma once

#include "mission_control.hpp"
#include <array>
#include <cinttypes>
#include <string_view>

#include <libhal-esp8266/at.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/streams.hpp>
#include <libhal-util/timeout.hpp>
#include <libhal/timeout.hpp>


namespace sjsu::drive {

class esp8266_mission_control : public mission_control
{
public:
  hal::result<esp8266_mission_control> create(hal::esp8266::at& p_esp8266,
    hal::serial& p_console,
    const std::string_view p_ssid,
    const std::string_view p_password,
    const hal::esp8266::at::socket_config& p_config,
    const std::string_view p_ip,
    const std::chrono::seconds p_time_to_timeout)
  {
    return esp8266_mission_control(p_esp8266, p_console,
    p_ssid, p_password, p_config, p_ip, p_time_to_timeout);
  }

private:

  esp8266_mission_control(hal::esp8266::at& p_esp8266,
    hal::serial& p_console,
    hal::steady_clock* clock,
    const std::string_view p_ssid,
    const std::string_view p_password,
    const hal::esp8266::at::socket_config& p_config,
    const std::string_view p_ip,
    const std::chrono::seconds p_time_to_timeout) : m_esp8266(&p_esp8266), m_console(p_console), 
      m_ssid(p_ssid), m_password(p_password), m_config(p_config), m_ip(p_ip), 
      m_time_to_timeout(p_time_to_timeout)
      {
      }

  drive_commands impl_get_command(
    hal::function_ref<hal::timeout_function> p_timeout) override
  {
    auto timeout = hal::create_timeout(clock, p_time_to_timeout);
    if(establish_connection(timeout)) {
      // read commands if no error was thrown, otherwise leave gracefully
      parse_commands();
    }

    return m_commands;
  }

  hal::status parse_commands() {}

  enum class connection_state
  {
    check_ap_connection,
    connecting_to_ap,
    set_ip_address,
    check_server_connection,
    connecting_to_server,
    connection_established,
  };

  [[nodiscard]] hal::status establish_connection(hal::timeout auto& p_timeout)
  {
    connection_state state = connection_state::check_ap_connection;

    while (state != connection_state::connection_established) {
      switch (state) {
        case connection_state::check_ap_connection:
          hal::print(m_console, "Checking if AP \"");
          hal::print(m_console, m_ssid);
          hal::print(m_console, "\" is connected... ");
          if (HAL_CHECK(m_esp8266.is_connected_to_ap(p_timeout))) {
            state = connection_state::check_server_connection;
            hal::print(m_console, "Connected!\n");
          } else {
            state = connection_state::connecting_to_ap;
            hal::print(m_console, "NOT Connected!\n");
          }
          break;
        case connection_state::connecting_to_ap:
          hal::print(m_console, "Connecting to AP: \"");
          hal::print(m_console, m_ssid);
          hal::print(m_console, "\" ...\n");
          HAL_CHECK(m_esp8266.connect_to_ap(m_ssid, m_password, p_timeout));
          state = connection_state::set_ip_address;
          break;
        case connection_state::set_ip_address:
          if (!p_ip.empty()) {
            hal::print(m_console, "Setting IP Address to: ");
            hal::print(m_console, m_ip);
            hal::print(m_console, " ...\n");
            HAL_CHECK(m_esp8266.set_ip_address(m_ip, p_timeout));
          }
          state = connection_state::check_server_connection;
          break;
        case connection_state::check_server_connection:
          hal::print(m_console, "Checking if server \"");
          hal::print(m_console, m_config.domain);
          hal::print(m_console, "\" is connected... ");
          if (HAL_CHECK(m_esp8266.is_connected_to_server(p_timeout))) {
            state = connection_state::connection_established;
            hal::print(m_console, "Connected!\n");
          } else {
            state = connection_state::connecting_to_server;
            hal::print(m_console, "NOT Connected!\n");
          }
          break;
        case connection_state::connecting_to_server:
          hal::print(m_console, "Connecting to server: \"");
          hal::print(m_console, m_config.domain);
          hal::print(m_console, "\" ...\n");
          HAL_CHECK(m_esp8266.connect_to_server(m_config, p_timeout));
          state = connection_state::check_server_connection;
          break;
        case connection_state::connection_established:
          // Do nothing, allow next iteration to break while loop
          break;
        default:
          state = connection_state::connecting_to_ap;
      }
    }

    return hal::success();
  }

  http_header_parser_t new_http_header_parser()
  {
    using namespace std::literals;

    return http_header_parser_t{
      .find_header_start = hal::stream::find(hal::as_bytes("HTTP/1.1 "sv)),
      .find_content_length =
        hal::stream::find(hal::as_bytes("Content-Length: "sv)),
      .parse_content_length = hal::stream::parse<std::uint32_t>(),
      .find_end_of_header = hal::stream::find(hal::as_bytes("\r\n\r\n"sv)),
    };
  }

  drive_commands m_commands{};
  hal::esp8266::at& m_esp8266;
  hal::serial& m_console;
  std::string_view m_ssid;
  std::string_view m_password;
  hal::esp8266::at::socket_config& m_config;
  std::string_view m_ip;
  std::chrono::seconds m_time_to_timeout;
};
} 