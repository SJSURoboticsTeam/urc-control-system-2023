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
#include "tcp_client.hpp"

namespace sjsu::drive {

class esp8266_mission_control : public mission_control
{
public:
  static hal::result<esp8266_mission_control> create(
                          tcp_client& client,
    // hal::esp8266::at& p_esp8266,
    hal::serial& p_console,
    // const std::string_view p_ssid,
    // const std::string_view p_password,
    // const hal::esp8266::at::socket_config& p_config,
    // const std::string_view p_ip,
    // hal::timeout auto& p_timeout,
    std::span<hal::byte> p_buffer,
    std::string_view p_get_request)
  {
    esp8266_mission_control esp_mission_control =
      esp8266_mission_control(
                              client,
                              // p_esp8266,
                              p_console,
                              // p_ssid,
                              // p_password,
                              // p_config,
                              // p_ip,
                              p_buffer,
                              p_get_request);
    // HAL_CHECK(esp_mission_control.establish_connection(p_timeout));

    return esp_mission_control;
  }

  void set_feedback(mc_feedback p_feedback) {
    m_feedback = p_feedback;
  }

private:
  mc_feedback m_feedback;
  esp8266_mission_control(
                          tcp_client& p_client,
                          // hal::esp8266::at& p_esp8266,
                          hal::serial& p_console,
                          // const std::string_view p_ssid,
                          // const std::string_view p_password,
                          // const hal::esp8266::at::socket_config& p_config,
                          // const std::string_view p_ip,
                          std::span<hal::byte> p_buffer,
                          std::string_view p_get_request) :
    // : m_esp8266(&p_esp8266)

    m_console(&p_console),
    m_client(&p_client)
    // , m_ssid(p_ssid)
    // , m_password(p_password)
    // , m_config(p_config)
    // , m_ip(p_ip)
    , m_buffer(p_buffer)
    , m_get_request(p_get_request)
    , m_fill_payload(hal::stream_fill(m_buffer))
    , m_http_header_parser(new_http_header_parser())
  {
    m_buffer_len = 0;
  }
  /*
  
  struct mc_feedback {
    float current_wheel_speed = 0.0;
    float current_steering_angle = 0.0;
    float current_wheel_heading = 0.0;

    float delta_wheel_speed = 0.0;
    float delta_steering_angle = 0.0;
    float delta_wheel_heading = 0.0;

    struct motor_feedback {
      float angle = 0.0f;
      float speed = 0.0f;
      float current = 0.0f;
    };
    struct leg_feedback {
      motor_feedback steering;
      motor_feedback propulsion;
      float requested_steering_angle = 0.0;
      float requested_propulsion_speed = 0.0;
    }; 

    leg_feedback fl, fr, b;
    float dt, mc_ping;
  };*/

  hal::status send_feedback(hal::function_ref<hal::timeout_function> p_timeout) {
      // auto status = m_client->send(hal::as_bytes(m_get_request), p_timeout);
    // constexpr std::string_view endpoint = "GET /drive?";
    // constexpr std::string_view endpoint_rest = " HTTP/1.1\r\nHost: 192.168.0.211:5000\r\nConnection:keep-alive\r\n\r\n";
    // m_client->send(hal::as_bytes(endpoint), p_timeout);
    char buffer[1024];
    // constexpr std::string_view bruh = "a=c";
    size_t n = snprintf(buffer, sizeof(buffer), get_request_format, 
      m_feedback.dt,

      m_feedback.current_wheel_speed,
      m_feedback.current_steering_angle,
      m_feedback.current_wheel_heading,

      m_feedback.delta_wheel_speed,
      m_feedback.delta_steering_angle,
      m_feedback.delta_wheel_heading,


      m_feedback.fl.steering.angle,
      m_feedback.fl.steering.speed,
      m_feedback.fl.steering.current,
      m_feedback.fl.propulsion.angle,
      m_feedback.fl.propulsion.speed,
      m_feedback.fl.propulsion.current,
      m_feedback.fl.requested_steering_angle,
      m_feedback.fl.requested_propulsion_speed,

      m_feedback.fr.steering.angle,
      m_feedback.fr.steering.speed,
      m_feedback.fr.steering.current,
      m_feedback.fr.propulsion.angle,
      m_feedback.fr.propulsion.speed,
      m_feedback.fr.propulsion.current,
      m_feedback.fr.requested_steering_angle,
      m_feedback.fr.requested_propulsion_speed,

      m_feedback.b.steering.angle,
      m_feedback.b.steering.speed,
      m_feedback.b.steering.current,
      m_feedback.b.propulsion.angle,
      m_feedback.b.propulsion.speed,
      m_feedback.b.propulsion.current,
      m_feedback.b.requested_steering_angle,
      m_feedback.b.requested_propulsion_speed
    );
    hal::print<2313>(*m_console, "%s\n", buffer);
    std::span<hal::byte> bytes{reinterpret_cast<hal::byte*>(std::addressof(buffer)), n}; 
    m_client->send(bytes, p_timeout);

    // m_client->send(hal::as_bytes(endpoint_rest), p_timeout);
    return hal::success();
  }

  hal::result<mc_commands> impl_get_command(
    hal::function_ref<hal::timeout_function> p_timeout) override
  {
    using namespace std::literals;

    // auto dumfuck = HAL_CHECK(m_esp8266->is_connected_to_server(p_timeout));
    if (m_write_error) {
      hal::print(*m_console, "Reconnecting...\n");
      // Wait 1s before attempting to reconnect

      // auto result = establish_connection(p_timeout);
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

      // auto status = m_client->send(hal::as_bytes(m_get_request), p_timeout);
        auto status = send_feedback(p_timeout);
        // m_esp8266->server_write(hal::as_bytes(m_get_request), p_timeout);

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
    // auto received = HAL_CHECK(m_esp8266->server_read(m_buffer)).data;
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

  void parse_commands() {

    auto result = to_string_view(m_command_buffer);
    static constexpr int expected_number_of_arguments = 6;
    // mc_commands commands;

    int actual_arguments = sscanf(result.data(),
                                  kResponseBodyFormat,
                                  &m_commands.heartbeat_count,
                                  &m_commands.is_operational,
                                  &m_commands.wheel_orientation,
                                  &m_commands.mode,
                                  &m_commands.speed,
                                  &m_commands.angle);

    if (actual_arguments != expected_number_of_arguments) {
      hal::print<200>(*m_console,
                      "Received %d arguments, expected %d\n",
                      actual_arguments,
                      expected_number_of_arguments);
    }

    // A hack to get the current mission control to work with the new steering.
    m_commands.wheel_speed = static_cast<float>(m_commands.speed);
    switch(m_commands.mode) {
    case 'D':
      // In drive mode, angle refers to steering angle.
      m_commands.steering_angle = m_commands.angle;
      m_commands.wheel_heading = 0.0;
      break;
    case 'S':
      // In spin mode, wheel heading should not change and steering angle should be 90
      m_commands.steering_angle = 90.0;
      m_commands.wheel_heading = 0.0; // Set it to 0 for now.
      break;
    case 'T':
      // In translate mode, angle refers to wheel heading.
      m_commands.wheel_heading = m_commands.angle;
      m_commands.steering_angle = 0.0;
      break;
    }

    // Increment the message count.
    m_commands.message_count ++; 
  }


  std::string_view to_string_view(std::span<const hal::byte> p_span)
  {
    return std::string_view(reinterpret_cast<const char*>(p_span.data()),
                            p_span.size());
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
  // hal::esp8266::at* m_esp8266;
  hal::serial* m_console;
  // std::string_view m_ssid;
  // std::string_view m_password;
  // const hal::esp8266::at::socket_config& m_config;
  // std::string_view m_ip;
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

  tcp_client* m_client;
};
}  // namespace sjsu::arm