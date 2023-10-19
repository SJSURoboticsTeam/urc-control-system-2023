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
  static hal::result<esp8266_mission_control> create(
    hal::esp8266::at& p_esp8266,
    hal::serial& p_console,
    const std::string_view p_ssid,
    const std::string_view p_password,
    const hal::esp8266::at::socket_config& p_config,
    const std::string_view p_ip,
    hal::timeout auto& p_timeout,
    std::span<hal::byte> p_buffer,
    std::string_view p_get_request)
  {
    esp8266_mission_control esp_mission_control =
      esp8266_mission_control(p_esp8266,
                              p_console,
                              p_ssid,
                              p_password,
                              p_config,
                              p_ip,
                              p_buffer,
                              p_get_request);
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
                          std::string_view p_get_request)
    : m_esp8266(&p_esp8266)
    , m_console(&p_console)
    , m_ssid(p_ssid)
    , m_password(p_password)
    , m_config(p_config)
    , m_ip(p_ip)
    , m_buffer(p_buffer)
    , m_get_request(p_get_request)
    , m_fill_payload(hal::stream_fill(m_buffer))
    , m_http_header_parser(new_http_header_parser())
  {
    m_buffer_len = 0;
  }

  hal::result<mc_commands> impl_get_command(
    hal::function_ref<hal::timeout_function> p_timeout) override
  {
    using namespace std::literals;

    // auto dumfuck = HAL_CHECK(m_esp8266->is_connected_to_server(p_timeout));
    if (m_write_error) {
      hal::print(*m_console, "Reconnecting...\n");
      // Wait 1s before attempting to reconnect

      auto result = establish_connection(p_timeout);
      if (!result) {
        hal::print(*m_console, "Failure!!!\n");
        return m_commands;
      }

      hal::print(*m_console, "CONNECTION RE-ESTABLISHED!!\n");
      m_read_complete = true;
      m_buffer_len = 0;
      m_content_length = 0;
      m_http_header_parser = new_http_header_parser();
      m_write_error = false;
      m_header_finished = false;
    }
    if (m_read_complete) {

      // Send out HTTP GET request

      auto status =
        m_esp8266->server_write(hal::as_bytes(m_get_request), p_timeout);

      if (!status) {
        hal::print(*m_console, "\nFailed to write to server!\n");
        hal::print(*m_console, m_get_request);
        m_write_error = true;
        return m_commands;
      }

      m_missed_read = 0;
      m_read_complete = false;
      m_header_finished = false;
    }
    auto received = HAL_CHECK(m_esp8266->server_read(m_buffer)).data;
    auto remainder = received | m_http_header_parser.find_header_start |
                     m_http_header_parser.find_content_length |
                     m_http_header_parser.parse_content_length |
                     m_http_header_parser.find_end_of_header;

    m_missed_read++;
    if (m_missed_read > 15) {
      hal::print(*m_console, "READ MISS!!!\n");
      m_write_error = true;
      return m_commands;
    }
    // auto dumfuck2 = HAL_CHECK(m_esp8266->is_connected_to_server(p_timeout));
    // if(dumfuck2){
    //   m_write_error = true;
    //   return m_commands;
    // }

    // hal::print<1024>(*m_console, "IS CONNECTED TO SERVER: %d\n", dumfuck2);
    // auto app_connection =
    // HAL_CHECK(m_esp8266->is_connected_to_app(p_timeout));

    if (!m_header_finished &&
        hal::finished(m_http_header_parser.find_end_of_header)) {
      m_content_length = m_http_header_parser.parse_content_length.value();
      m_header_finished = true;
      std::fill(m_command_buffer.begin(), m_command_buffer.end(), 0);
      remainder = remainder.subspan(1);
    }

    if (m_header_finished) {
      hal::print<128>(*m_console, " read miss = %u\n", m_missed_read);

      auto tmp = m_content_length - m_buffer_len;
      auto byte_to_read = std::min((size_t)tmp, remainder.size());

      for (int i = 0; i < byte_to_read; i++) {
        m_command_buffer[m_buffer_len + i] = remainder[i];
      }
      m_buffer_len += byte_to_read;
      hal::print<1024>(*m_console,
                       "M header has finished, remainder size: %d, buffer "
                       "data: %.*s, buffer len %d, content length %d\n",
                       remainder.size(),
                       m_buffer_len + 1,
                       m_command_buffer.data(),
                       m_buffer_len,
                       m_content_length);

      if (m_buffer_len >= m_content_length) {
        hal::print(*m_console, "content length has been met \n");
        m_read_complete = true;
        m_buffer_len = 0;
        parse_commands();
        m_http_header_parser = new_http_header_parser();
      }
    }
    return m_commands;
  }

  void parse_commands()
  {
    auto result = to_string_view(m_command_buffer);

    static constexpr int expected_number_of_arguments = 9;
    sjsu::drive::mission_control::mc_commands commands;
    int actual_arguments = sscanf(result.data(),
                                  kResponseBodyFormat,
                                  &commands.heartbeat_count,
                                  &commands.is_operational,
                                  &commands.speed,
                                  &commands.rotunda_angle,
                                  &commands.shoulder_angle,
                                  &commands.elbow_angle,
                                  &commands.wrist_pitch_angle,
                                  &commands.wrist_roll_angle,
                                  &commands.rr9_angle);
    if (actual_arguments != expected_number_of_arguments) {
      hal::print<2048>(*m_console,
                       "Received %d arguments, expected %d\n",
                       actual_arguments,
                       expected_number_of_arguments);
      return;
    }
    hal::print<200>(
      *m_console,
      "HB: %d,\n IO %d,\n Speed: %d,\n Rotuda: %d,\n Shoulder: %d,\n Elbow: "
      "%d,\n WR_Pitch: %d,\n WR_Roll: %d,\n Endo: %d\n",
      commands.heartbeat_count,
      commands.is_operational,
      commands.speed,
      commands.rotunda_angle,
      commands.shoulder_angle,
      commands.elbow_angle,
      commands.wrist_pitch_angle,
      commands.wrist_roll_angle,
      commands.rr9_angle);
    m_commands = commands;
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
    hal::stream_find find_header_start;
    hal::stream_find find_content_length;
    hal::stream_parse<std::uint32_t> parse_content_length;
    hal::stream_find find_end_of_header;
  };

  http_header_parser_t new_http_header_parser()
  {
    using namespace std::literals;

    return http_header_parser_t{
      .find_header_start = hal::stream_find(hal::as_bytes("HTTP/1.1 "sv)),
      .find_content_length =
        hal::stream_find(hal::as_bytes("Content-Length: "sv)),
      .parse_content_length = hal::stream_parse<std::uint32_t>(),
      .find_end_of_header = hal::stream_find(hal::as_bytes("\r\n\r\n"sv)),
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
  std::array<hal::byte, 128> m_command_buffer;
  std::string_view m_get_request;
  http_header_parser_t m_http_header_parser;
  size_t m_buffer_len;
  bool m_write_error = false;
  bool m_header_finished = false;
  bool m_read_complete = true;
  hal::stream_fill m_fill_payload;
  size_t m_content_length;
  std::uint32_t m_missed_read = 0;
};
}  // namespace sjsu::arm