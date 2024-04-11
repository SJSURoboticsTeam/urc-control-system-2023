#include <array>
#include <cinttypes>
#include <string_view>

#include <libhal-esp8266/at.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/streams.hpp>
#include <libhal-util/timeout.hpp>
#include <libhal/timeout.hpp>

#include "../include/esp8266_mission_control.hpp"
#include "../include/mission_control.hpp"

namespace sjsu::drive {

esp8266_mission_control::esp8266_mission_control(
  hal::esp8266::at& p_esp8266,
  hal::serial& p_console,
  const std::string_view p_ssid,
  const std::string_view p_password,
  const hal::esp8266::at::socket_config& p_config,
  const std::string_view p_ip,
  std::span<hal::byte> p_buffer,
  std::string_view p_get_request,
  hal::function_ref<hal::timeout_function> p_timeout)
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
  , m_timeout(p_timeout)

  , m_buffer_len(0)
{
  this->establish_connection(p_timeout);
}

mission_control::mc_commands
esp8266_mission_control::impl_get_command(
  hal::function_ref<hal::timeout_function> p_timeout)
{
    using namespace std::literals;
    hal::print(*m_console, "In get command...\n");

    // auto dumfuck = m_esp8266->is_connected_to_server(p_timeout));
    if (m_write_error) {
      hal::print(*m_console, "Reconnecting...\n");
      // Wait 1s before attempting to reconnect

      try {
      establish_connection(p_timeout);
      } catch (const hal::timed_out&) {
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
      try {
      hal::print(*m_console, "in the tryyyy...\n");

      auto status =
        m_esp8266->server_write(hal::as_bytes(m_get_request), p_timeout);
        
      } catch (...) {
        hal::print(*m_console, "\nFailed to write to server!\n");
        hal::print(*m_console, m_get_request);
        m_write_error = true;
        return m_commands;
      }
      m_missed_read = 0;
      m_read_complete = false;
      m_header_finished = false;
    }
    auto received = m_esp8266->server_read(m_buffer);
    auto remainder = received | m_http_header_parser.find_header_start |
                     m_http_header_parser.find_content_length |
                     m_http_header_parser.parse_content_length |
                     m_http_header_parser.find_end_of_header;

    m_missed_read++;
    if (m_missed_read > 10) {
      hal::print(*m_console, "READ MISS!!!\n");
      m_write_error = true;
      return m_commands;
    }

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

      for (uint32_t i = 0; i < byte_to_read; i++) {
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
        m_commands.print(m_console);
        m_http_header_parser = new_http_header_parser();
      }
    }
    return m_commands;

}

void esp8266_mission_control::parse_commands()
{

  auto result = to_string_view(m_command_buffer);
  static constexpr int expected_number_of_arguments = 6;
  mc_commands commands;

  int actual_arguments = sscanf(result.data(),
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
  }

  m_commands = commands;
}

esp8266_mission_control::http_header_parser_t
esp8266_mission_control::new_http_header_parser()
{
  using namespace std::literals;

  return esp8266_mission_control::http_header_parser_t{
    .find_header_start = hal::stream_find(hal::as_bytes("HTTP/1.1 "sv)),
    .find_content_length =
      hal::stream_find(hal::as_bytes("Content-Length: "sv)),
    .parse_content_length = hal::stream_parse<std::uint32_t>(),
    .find_end_of_header = hal::stream_find(hal::as_bytes("\r\n\r\n"sv)),
  };
}

std::string_view esp8266_mission_control::to_string_view(
  std::span<const hal::byte> p_span)
{
  return std::string_view(reinterpret_cast<const char*>(p_span.data()),
                          p_span.size());
}

void esp8266_mission_control::establish_connection(
  hal::function_ref<hal::timeout_function> p_timeout)
{
  connection_state state = connection_state::check_ap_connection;

  while (state != connection_state::connection_established) {
    switch (state) {
      case connection_state::check_ap_connection:
        hal::print(*m_console, "Checking if AP \"");
        hal::print(*m_console, m_ssid);
        hal::print(*m_console, "\" is connected... ");
        if (m_esp8266->is_connected_to_ap(p_timeout)) {
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
        m_esp8266->connect_to_ap(m_ssid, m_password, p_timeout);
        state = connection_state::set_ip_address;
        break;
      case connection_state::set_ip_address:
        if (!m_ip.empty()) {
          hal::print(*m_console, "Setting IP Address to: ");
          hal::print(*m_console, m_ip);
          hal::print(*m_console, " ...\n");
          m_esp8266->set_ip_address(m_ip, p_timeout);
        }
        state = connection_state::check_server_connection;
        break;
      case connection_state::check_server_connection:
        hal::print(*m_console, "Checking if server \"");
        hal::print(*m_console, m_config.domain);
        hal::print(*m_console, "\" is connected... \n");
        if (m_esp8266->is_connected_to_server(p_timeout)) {
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
        m_esp8266->connect_to_server(m_config, p_timeout);
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

}

}  // namespace sjsu::drive