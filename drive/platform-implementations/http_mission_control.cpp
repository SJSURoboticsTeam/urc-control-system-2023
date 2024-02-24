#include <array>
#include <cinttypes>
#include <string_view>

#include <libhal-esp8266/at.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/streams.hpp>
#include <libhal-util/timeout.hpp>
#include <libhal/timeout.hpp>

#include "../include/http_mission_control.hpp"
#include "../include/mission_control.hpp"

namespace sjsu::drive {

http_mission_control::http_mission_control(
  tcp_client& p_client,
  hal::serial& p_console,
  std::span<hal::byte> p_buffer,
  std::string_view p_get_request)
  : m_client(&p_client)
  , m_console(&p_console)
  , m_buffer(p_buffer)
  , m_get_request(p_get_request)
  , m_fill_payload(hal::stream_fill(m_buffer))
  , m_http_header_parser(new_http_header_parser())
  , m_buffer_len(0)
{
}

hal::result<mission_control::mc_commands>
http_mission_control::impl_get_command(
  hal::function_ref<hal::timeout_function> p_timeout)
{
  using namespace std::literals;

  // auto dumfuck = HAL_CHECK(m_esp8266->is_connected_to_server(p_timeout));
  if (m_write_error) {
    hal::print(*m_console, "Reconnecting...\n");
    // Wait 1s before attempting to reconnect

    auto result = m_client->reestablish_connection(p_timeout);
    
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

    auto status = m_client->send(hal::as_bytes(m_get_request), p_timeout);

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
  auto received = HAL_CHECK(m_client->recieve(m_buffer, p_timeout));
  auto remainder = received | m_http_header_parser.find_header_start |
                   m_http_header_parser.find_content_length |
                   m_http_header_parser.parse_content_length |
                   m_http_header_parser.find_end_of_header;

  m_missed_read++;
  if (m_missed_read > 10) {
    hal::print(*m_console, "READ MISS!!!\n");
    m_write_error = true;
    m_missed_read = 0;
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
    // hal::print<128>(*m_console, " read miss = %u\n", m_missed_read);

    auto tmp = m_content_length - m_buffer_len;
    auto byte_to_read = std::min((size_t)tmp, remainder.size());

    for (int i = 0; i < byte_to_read; i++) {
      m_command_buffer[m_buffer_len + i] = remainder[i];
    }
    m_buffer_len += byte_to_read;
    // hal::print<1024>(*m_console,
    //                  "M header has finished, remainder size: %d, buffer "
    //                  "data: %.*s, buffer len %d, content length %d\n",
    //                  remainder.size(),
    //                  m_buffer_len + 1,
    //                  m_command_buffer.data(),
    //                  m_buffer_len,
    //                  m_content_length);

    if (m_buffer_len >= m_content_length) {
      // hal::print(*m_console, "content length has been met \n");
      m_read_complete = true;
      m_missed_read = 0;
      m_buffer_len = 0;
      parse_commands();
      m_http_header_parser = new_http_header_parser();
    }
  }
  return m_commands;
}

void http_mission_control::parse_commands()
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

http_mission_control::http_header_parser_t
http_mission_control::new_http_header_parser()
{
  using namespace std::literals;

  return http_mission_control::http_header_parser_t{
    .find_header_start = hal::stream_find(hal::as_bytes("HTTP/1.1 "sv)),
    .find_content_length =
      hal::stream_find(hal::as_bytes("Content-Length: "sv)),
    .parse_content_length = hal::stream_parse<std::uint32_t>(),
    .find_end_of_header = hal::stream_find(hal::as_bytes("\r\n\r\n"sv)),
  };
}

std::string_view http_mission_control::to_string_view(
  std::span<const hal::byte> p_span)
{
  return std::string_view(reinterpret_cast<const char*>(p_span.data()),
                          p_span.size());
}
}  // namespace sjsu::drive
