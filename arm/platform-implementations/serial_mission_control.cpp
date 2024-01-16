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

namespace sjsu::arm {

class serial_mission_control : public mission_control
{
public:
  static hal::result<serial_mission_control> create(
    hal::serial& p_console,
    std::span<hal::byte> p_buffer
    )
  {
    serial_mission_control serial_mission_control =
      serial_mission_control(p_console,p_buffer);

    return serial_mission_control;
  }

private:
//   esp8266_mission_control(hal::esp8266::at& p_esp8266,
//                           hal::serial& p_console,
//                           const std::string_view p_ssid,
//                           const std::string_view p_password,
//                           const hal::esp8266::at::socket_config& p_config,
//                           const std::string_view p_ip,
//                           std::span<hal::byte> p_buffer,
//                           std::string_view p_get_request)
//     : m_esp8266(&p_esp8266)
//     , m_console(&p_console)
//     , m_ssid(p_ssid)
//     , m_password(p_password)
//     , m_config(p_config)
//     , m_ip(p_ip)
//     , m_buffer(p_buffer)
//     , m_get_request(p_get_request)
//     , m_fill_payload(hal::stream_fill(m_buffer))
//     , m_http_header_parser(new_http_header_parser())
//   {
//     m_buffer_len = 0;
//   }
    serial_mission_control(hal::serial& p_console,
                           std::span<hal::byte> p_buffer)
                           :m_console(&p_console)
                           ,m_buffer(p_buffer){
                            m_buffer_len = 0;
                           }


  hal::result<mc_commands> impl_get_command(
    hal::function_ref<hal::timeout_function> p_timeout) override
  {
    using namespace std::literals;
    auto readings = HAL_CHECK(m_console.read(m_buffer));
    //start = { end = }
    m_command_buffer = readings.data;

    parse_commands();

    return m_commands;
  }

  void parse_commands()
  {
    auto result = to_string_view(m_command_buffer);
    // result = result.substr(result.find('{'));
    if (result.find('{') == std::string_view::npos){
      return;
    }
    result = result.substr(result.find('{') );
    static constexpr int expected_number_of_arguments = 9;
    sjsu::arm::mission_control::mc_commands commands;
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
    
    m_commands = commands;
  }

  std::string_view to_string_view(std::span<const hal::byte> p_span)
  {
    return std::string_view(reinterpret_cast<const char*>(p_span.data()),
                            p_span.size());
  }



  mc_commands m_commands{};
  hal::serial* m_console;

  std::span<hal::byte> m_buffer;
  std::array<hal::byte, 128> m_command_buffer;
  size_t m_buffer_len;

};
}  // namespace sjsu::arm