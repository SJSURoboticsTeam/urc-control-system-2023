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
    static hal::result<esp8266_mission_control> create(hal::esp8266::at& p_esp8266,
    hal::serial& p_console,
    const std::string_view p_ssid,
    const std::string_view p_password,
    const hal::esp8266::at::socket_config& p_config,
    const std::string_view p_ip,
    hal::timeout auto& p_timeout, 
    std::span<hal::byte> p_buffer,
    std::string_view p_get_request)
  {
    esp8266_mission_control esp_mission_control = esp8266_mission_control(p_esp8266, p_console,
    p_ssid, p_password, p_config, p_ip, p_buffer, p_get_request);
    HAL_CHECK(esp_mission_control.establish_connection(p_timeout));

    return esp_mission_control;
  }

private:

  esp8266_mission_control(hal::esp8266::at& p_esp8266,
    hal::serial& p_console,
    const std::string_view p_ssid,
    const std::string_view p_password,
    const hal::esp8266::at::socket_config& p_config,
    const std::string_view p_ip,
    std::span<hal::byte> p_buffer,
    std::string_view p_get_request) : m_esp8266(&p_esp8266), m_console(&p_console), 
      m_ssid(p_ssid), m_password(p_password), m_config(p_config), m_ip(p_ip),
      m_buffer(p_buffer), m_get_request(p_get_request)
      {
      }

  hal::result<mc_commands> impl_get_command(
    hal::function_ref<hal::timeout_function> p_timeout) override
  {

    using namespace std::literals;

    auto http_header_parser = new_http_header_parser();
    bool write_error = false;
    bool header_finished = false;
    bool read_complete = true;
    auto fill_payload = hal::stream::fill(m_buffer);
    if (write_error) {
      hal::print(*m_console, "Reconnecting...\n");
      // Wait 1s before attempting to reconnect

      auto result = establish_connection(p_timeout);
      if (!result) {
        return m_commands;
      }
      write_error = false;
    }

    if (read_complete) {

      // Send out HTTP GET request
      auto status = m_esp8266->server_write(hal::as_bytes(m_get_request), p_timeout);
      if (!status) {
        hal::print(*m_console, "\nFailed to write to server!\n");
        write_error = true;
        return m_commands;
      }

      read_complete = false;
      header_finished = false;
    }

    auto received = HAL_CHECK(m_esp8266->server_read(m_buffer)).data;
    auto remainder = received | http_header_parser.find_header_start |
                     http_header_parser.find_content_length |
                     http_header_parser.parse_content_length |
                     http_header_parser.find_end_of_header;
    
    if (!header_finished &&
        hal::finished(http_header_parser.find_end_of_header)) {
      auto content_length = http_header_parser.parse_content_length.value();
      fill_payload = hal::stream::fill(m_buffer, content_length);
      header_finished = true;
    }

    if (header_finished && hal::in_progress(fill_payload)) {
      remainder | fill_payload;
      hal::print(*m_console, "header finished and fill payload in progress\n");
      if (hal::finished(fill_payload.state())) {
        hal::print(*m_console, "fill payload finished\n");
        m_commands = HAL_CHECK(parse_commands());
        hal::print(*m_console, "finished parsing comamnds\n");
        read_complete = true;
        http_header_parser = new_http_header_parser();
        fill_payload = hal::stream::fill(m_buffer);
      }
    }

    return m_commands;
  }

  hal::result<mc_commands> parse_commands() {

    auto result = to_string_view(m_buffer);

    auto start = result.find('{');
    auto end = result.find('}');
    auto response = result.substr(start, end - start + 1);
    static constexpr int expected_number_of_arguments = 6;
    mc_commands commands;
    response = response.substr(response.find('{'));
    int actual_arguments = sscanf(response.data(),
                                  kResponseBodyFormat,
                                  &commands.heartbeat_count,
                                  &commands.is_operational,
                                  &commands.wheel_orientation,
                                  &commands.mode,
                                  &commands.speed,
                                  &commands.angle);
    if (actual_arguments != expected_number_of_arguments) {
      hal::print<200>(*m_console,
                      "Received %d arguments, expected %d\n",
                      actual_arguments,
                      expected_number_of_arguments);
      
      return m_commands;
    }
    hal::print<200>(*m_console,
                      "HB: %d\t, IO %d\t, WO: %d\t, DM: %c\t, Speed: %d\n, Angle: %d\n",
                      commands.heartbeat_count,
                      commands.is_operational,
                      commands.wheel_orientation,
                      commands.mode,
                      commands.speed,
                      commands.angle
                      );
    return commands;
  }

  std::string_view to_string_view(std::span<const hal::byte> p_span)
  {
    return std::string_view(reinterpret_cast<const char*>(p_span.data()),
                            p_span.size());
  }

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
      hal::print(*m_console, "loop\n");
      switch (state) {
        case connection_state::check_ap_connection:
          hal::print(*m_console, "Checking if AP \"");
          hal::print(*m_console, m_ssid);
          hal::print(*m_console, "\" is connected... ");
          if (HAL_CHECK(m_esp8266->is_connected_to_ap(p_timeout))) {
            state = connection_state::check_server_connection;
            hal::print(*m_console, "Connected!\n");
          } else {
            state = connection_state::connecting_to_ap;
            hal::print(*m_console, "NOT Connected!\n");
          }
          break;
        case connection_state::connecting_to_ap:
          hal::print(*m_console, "Connecting to AP: \"");
          hal::print(*m_console, m_ssid);
          hal::print(*m_console, "\" ...\n");
          HAL_CHECK(m_esp8266->connect_to_ap(m_ssid, m_password, p_timeout));
          state = connection_state::set_ip_address;
          break;
        case connection_state::set_ip_address:
          if (!m_ip.empty()) {
            hal::print(*m_console, "Setting IP Address to: ");
            hal::print(*m_console, m_ip);
            hal::print(*m_console, " ...\n");
            HAL_CHECK(m_esp8266->set_ip_address(m_ip, p_timeout));
          }
          state = connection_state::check_server_connection;
          break;
        case connection_state::check_server_connection:
          hal::print(*m_console, "Checking if server \"");
          hal::print(*m_console, m_config.domain);
          hal::print(*m_console, "\" is connected... \n");
          if (HAL_CHECK(m_esp8266->is_connected_to_server(p_timeout))) {
            state = connection_state::connection_established;
            hal::print(*m_console, "Connected!\n");
          } else {
            state = connection_state::connecting_to_server;
            hal::print(*m_console, "NOT Connected!\n");
          }
          break;
        case connection_state::connecting_to_server:
          hal::print(*m_console, "Connecting to server: \"");
          hal::print(*m_console, m_config.domain);
          hal::print(*m_console, "\" ...\n");
          HAL_CHECK(m_esp8266->connect_to_server(m_config, p_timeout));
          hal::print(*m_console, "connected\n");
          state = connection_state::check_server_connection;
          break;
        case connection_state::connection_established:
          // Do nothing, allow next iteration to break while loop
          hal::print(*m_console, "Succesfully Connected.");
          break;
        default:
          state = connection_state::connecting_to_ap;
      }
    }

    return hal::success();
  }

  struct http_header_parser_t
  {
    hal::stream::find find_header_start;
    hal::stream::find find_content_length;
    hal::stream::parse<std::uint32_t> parse_content_length;
    hal::stream::find find_end_of_header;
  };

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

  mc_commands m_commands{};
  hal::esp8266::at* m_esp8266;
  hal::serial* m_console;
  std::string_view m_ssid;
  std::string_view m_password;
  const hal::esp8266::at::socket_config& m_config;
  std::string_view m_ip;
  std::span<hal::byte> m_buffer;
  std::string_view m_get_request;
};
} 