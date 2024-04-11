#include "esp8266_mission_control.hpp"
#include "mission_control.hpp"
#include <algorithm>
#include <array>
#include <libhal-util/as_bytes.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/timeout.hpp>
#include <libhal/error.hpp>
#include <string_view>
#include <type_traits>

namespace sjsu::arm {
const char kResponseBodyFormat[] =
  "{\"heartbeat_count\":%d,\"is_operational\":%d,\"speed\":%d,\"angles\":[%d,%"
  "d,%d,%d,%d,%d]}\n";
const char kGETRequestFormat[] =
  "arm?heartbeat_count=%d&is_operational=%d&speed=%d&rotunda_angle=%d&shoulder_"
  "angle=%d&elbow_angle=%d&wrist_pitch_angle=%d&wrist_roll_angle=%d&rr9_angle=%"
  "d";

esp8266_mission_control::esp8266_mission_control(
  hal::esp8266::at* p_esp8266,
  hal::serial* p_console,
  access_point_credentials_t p_credentials,
  hal::steady_clock* p_clock)
  : m_esp8266(p_esp8266)
  , m_console(p_console)
  , m_clock(p_clock)
  , m_credentials(p_credentials)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;
  auto timeout = hal::create_timeout(*m_clock, 100ms);
  try{
    this->establish_connection(timeout);
    this->write_get_request();
  }
  catch(...){
    
  }
}
#if 0
std::string_view esp8266_mission_control::read_from_server()
{
  using namespace std::chrono_literals;
  using namespace hal::literals;
  std::string_view commands;
  try {
    auto timeout = hal::create_timeout(*m_clock, 100ms);
    if (!m_esp8266->is_connected_to_server(timeout)) {
      establish_connection(timeout);
    }
    char* valid_data = nullptr;

      [[maybe_unused]] auto _ = m_esp8266->server_write(
        hal::as_bytes(m_credentials.m_get_request), timeout);
    std::array<hal::byte, 1024> command_buffer;
    command_buffer.fill(0);
    while(true){
      std::array<hal::byte, 128> tmp_buffer;
      tmp_buffer.fill(0);
      auto received = m_esp8266->server_read(tmp_buffer);
      // filter out the unusable http header
      auto remainder =
        received | m_http_header_parser.find_header_start |
        m_http_header_parser.find_content_length |
        m_http_header_parser.parse_content_length |
        m_http_header_parser.find_end_of_header;

      if (remainder.size() != 0) {
        //itterate the remainders into the command buffer until we reach parse length

      }
      timeout();
    }
    if(valid_data == nullptr){
      hal::safe_throw(hal::resource_unavailable_try_again(this));
    }
  } catch (...) {
    // if esp times out or server breaks down, return the old commands
    commands = "read miss";
  }
  return commands;
}

mc_commands esp8266_mission_control::impl_get_command() override
{
  using namespace std::literals;

  uint32_t missed_read = 0;
  missed_read++;
  if (missed_read > 15) {
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
    hal::print<128>(*m_console, " read miss = %u\n", missed_read);

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

void esp8266_mission_control::parse_commands(std::array<hal::byte, 1024> p_buffer)
{
  auto result = to_string_view(p_buffer);

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
#endif
esp8266_mission_control::mc_commands esp8266_mission_control::impl_get_command()
{
  using namespace std::chrono_literals;
  auto server_timeout = hal::create_timeout(*m_clock, 100ms);
  if (m_esp8266->is_connected_to_server(server_timeout)) {
    establish_connection(server_timeout);
  }
  std::array<hal::byte, 512> buffer{};
  auto json = m_esp8266->server_read(buffer);
  // The | operator acts a pipe, this is curretly piping the json into the
  // parsing format This should store all usable data into parsers _value
  // variables
  json | m_parser.find_header_start | m_parser.find_end_of_header |
    m_parser.find_heartbeat | m_parser.heartbeat_value |
    m_parser.find_operational | m_parser.operational_value |
    m_parser.find_speed | m_parser.speed_value | m_parser.find_angle_start |
    m_parser.rotunda_value | m_parser.find_array_divider1 |
    m_parser.shoulder_value | m_parser.find_array_divider2 |
    m_parser.elbow_value | m_parser.find_array_divider3 |
    m_parser.wrist_pitch_value | m_parser.find_array_divider4 |
    m_parser.wrist_roll_value | m_parser.find_array_divider5 |
    m_parser.endofactor_value | m_parser.find_array_end;

  // this will only update the commands when the full json is constructed
  if (hal::finished(m_parser.find_array_end)) {
    m_commands.heartbeat_count = m_parser.heartbeat_value.value();
    m_commands.is_operational = m_parser.operational_value.value();
    m_commands.speed = 1;
    m_commands.rotunda_angle = m_parser.rotunda_value.value();
    m_commands.shoulder_angle = m_parser.shoulder_value.value();
    m_commands.elbow_angle = m_parser.elbow_value.value();
    m_commands.wrist_pitch_angle = m_parser.wrist_roll_value.value();
    m_commands.wrist_roll_angle = m_parser.wrist_pitch_value.value();
    m_commands.rr9_angle = m_parser.endofactor_value.value();
    write_get_request();
  }
  return m_commands;
}
std::string_view esp8266_mission_control::to_string_view(
  std::span<const hal::byte> p_span)
{
  return std::string_view(reinterpret_cast<const char*>(p_span.data()),
                          p_span.size());
}

void esp8266_mission_control::establish_connection(hal::timeout auto& p_timeout)
{
  connection_state state = connection_state::check_ap_connection;

  while (state != connection_state::connection_established) {
    p_timeout();
    switch (state) {
      case connection_state::check_ap_connection:
        hal::print(*m_console, "Checking if AP \"");
        hal::print(*m_console, m_credentials.m_ssid);
        hal::print(*m_console, "\" is connected... ");
        if (m_esp8266->is_connected_to_ap(p_timeout)) {
          state = connection_state::set_ip_address;
          hal::print(*m_console, "Connected!\n");
        } else {
          state = connection_state::connecting_to_ap;
          hal::print(*m_console, "NOT Connected!\n");
        }
        break;
      case connection_state::connecting_to_ap:
        hal::print(*m_console, "Connecting to AP: \"");
        hal::print(*m_console, m_credentials.m_ssid);
        hal::print(*m_console, "\" ...\n");
        m_esp8266->connect_to_ap(
          m_credentials.m_ssid, m_credentials.m_password, p_timeout);
        state = connection_state::check_ap_connection;
        break;

      case connection_state::set_ip_address:
        if (!m_credentials.m_ip.empty()) {
          hal::print(*m_console, "Setting IP Address to: ");
          hal::print(*m_console, m_credentials.m_ip);
          hal::print(*m_console, " ...\n");
          m_esp8266->set_ip_address(m_credentials.m_ip, p_timeout);
        }
        state = connection_state::check_server_connection;
        break;
      case connection_state::check_server_connection:
        hal::print(*m_console, "Checking if server \"");
        hal::print(*m_console, m_credentials.m_config.domain);
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
        hal::print(*m_console, m_credentials.m_config.domain);
        hal::print(*m_console, "\" ...\n");
        m_esp8266->connect_to_server(m_credentials.m_config, p_timeout);
        hal::print(*m_console, "connected\n");
        state = connection_state::check_server_connection;
        break;
      case connection_state::connection_established:
        hal::print(*m_console, "Succesfully Connected.");
        break;
      default:
        state = connection_state::connecting_to_ap;
    }
  }
}
void esp8266_mission_control::write_get_request(){
  using namespace std::chrono_literals;
  auto server_write_deadline = hal::create_timeout(*m_clock, 100ms);
  [[maybe_unused]] auto _ = m_esp8266->server_write(
    hal::as_bytes(m_credentials.m_get_request), server_write_deadline);
}
}  // namespace sjsu::arm