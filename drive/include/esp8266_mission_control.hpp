#pragma once

#include <array>
#include <cinttypes>
#include <string_view>

#include "mission_control.hpp"
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
esp8266_mission_control(hal::esp8266::at& p_esp8266,
                          hal::serial& p_console,
                          const std::string_view p_ssid,
                          const std::string_view p_password,
                          const hal::esp8266::at::socket_config& p_config,
                          const std::string_view p_ip,
                          std::span<hal::byte> p_buffer,
                          std::string_view p_get_request);
  struct create_t
  {
    hal::esp8266::at& esp8266;
    hal::serial& console;
    const std::string_view ssid;
    const std::string_view password;
    const hal::esp8266::at::socket_config& config;
    const std::string_view ip;
    std::span<hal::byte> buffer;
    std::string_view get_request;
  };

  struct http_header_parser_t
  {
    hal::stream_find find_header_start;
    hal::stream_find find_content_length;
    hal::stream_parse<std::uint32_t> parse_content_length;
    hal::stream_find find_end_of_header;
  };

  enum class connection_state
  {
    check_ap_connection,
    connecting_to_ap,
    set_ip_address,
    check_server_connection,
    connecting_to_server,
    connection_established,
  };


private:
  

  mission_control::mc_commands impl_get_command(
    hal::function_ref<hal::timeout_function> p_timeout) override;

  void parse_commands();

  std::string_view to_string_view(std::span<const hal::byte> p_span);

  void establish_connection(
    hal::function_ref<hal::timeout_function> p_timeout);

  http_header_parser_t new_http_header_parser();

  hal::esp8266::at* m_esp8266;
  hal::serial* m_console;
  std::string_view m_ssid;
  std::string_view m_password;
  const hal::esp8266::at::socket_config& m_config;
  std::string_view m_ip;
  std::span<hal::byte> m_buffer;
  std::string_view m_get_request;
  hal::stream_fill m_fill_payload;
  http_header_parser_t m_http_header_parser;
  std::array<hal::byte, 128> m_command_buffer;
  size_t m_buffer_len;
  mc_commands m_commands{};
  bool m_write_error = false;
  bool m_header_finished = false;
  bool m_read_complete = true;
  size_t m_content_length;
  std::uint32_t m_missed_read = 0;

};

}  // namespace sjsu::drive
