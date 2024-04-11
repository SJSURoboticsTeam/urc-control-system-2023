#include <cstdint>
#include <libhal-util/as_bytes.hpp>
#include <libhal/error.hpp>
#include <libhal/steady_clock.hpp>
#include <string_view>

#include <libhal-esp8266/at.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/streams.hpp>
#include <libhal-util/timeout.hpp>
#include <libhal/timeout.hpp>

#include "mission_control.hpp"

namespace sjsu::arm {

class esp8266_mission_control : public mission_control
{
public:
  struct access_point_credentials_t
  {
    std::string_view m_ssid;
    std::string_view m_password;
    hal::esp8266::at::socket_config& m_config;
    std::string_view m_ip;
    std::string_view m_get_request;
  };

  esp8266_mission_control(hal::esp8266::at* p_esp8266,
                          hal::serial* p_console,
                          access_point_credentials_t p_credentials,
                          hal::steady_clock* p_clock);

  ~esp8266_mission_control() = default;

private:
  mc_commands impl_get_command() override;
  std::string_view to_string_view(std::span<const hal::byte> p_span);

  enum class connection_state
  {
    check_ap_connection,
    connecting_to_ap,
    set_ip_address,
    check_server_connection,
    connecting_to_server,
    connection_established,
  };

  void write_get_request();

  void establish_connection(hal::timeout auto& p_timeout);

  struct parser_t
  {

    // const char kResponseBodyFormat[] =
    //   "{\"heartbeat_count\":%d,\"is_operational\":%d,\"speed\":%d,\"angles\":[%d,%"
    //   "d,%d,%d,%d,%d]}\n";
    static constexpr std::string_view header_start = "HTTP/1.1 ";
    static constexpr std::string_view header_end = "\r\n\r\n";
    static constexpr std::string_view heartbeat = "{\"heartbeat_count\"";
    static constexpr std::string_view is_operational = "\"is_operational\"";
    static constexpr std::string_view speed = "\"speed\"";
    static constexpr std::string_view angles_start = "\"angles\":[";
    static constexpr std::string_view array_divider = ",";
    static constexpr std::string_view array_end = "]}\n";
    hal::stream_find find_header_start{hal::as_bytes(header_start)};
    hal::stream_find find_end_of_header {hal::as_bytes(header_end)};

    hal::stream_find find_heartbeat{hal::as_bytes(heartbeat)};
    hal::stream_parse<std::uint32_t> heartbeat_value{};

    hal::stream_find find_operational{hal::as_bytes(is_operational)};
    hal::stream_parse<std::uint32_t> operational_value{};

    hal::stream_find find_speed{hal::as_bytes(speed)};
    hal::stream_parse<std::uint32_t> speed_value{};
    
    hal::stream_find find_angle_start{hal::as_bytes(angles_start)};
    hal::stream_parse<std::uint32_t> rotunda_value{};
  
    hal::stream_find find_array_divider1{hal::as_bytes(array_divider)};
    hal::stream_parse<std::uint32_t> shoulder_value{};

    hal::stream_find find_array_divider2{hal::as_bytes(array_divider)};
    hal::stream_parse<std::uint32_t> elbow_value{};

    hal::stream_find find_array_divider3{hal::as_bytes(array_divider)};
    hal::stream_parse<std::uint32_t> wrist_pitch_value{};

    hal::stream_find find_array_divider4{hal::as_bytes(array_divider)};
    hal::stream_parse<std::uint32_t> wrist_roll_value{};

    hal::stream_find find_array_divider5{hal::as_bytes(array_divider)};
    hal::stream_parse<std::uint32_t> endofactor_value{};

    hal::stream_find find_array_end {hal::as_bytes(array_end)};
  };

  hal::esp8266::at* m_esp8266;
  hal::serial* m_console;
  hal::steady_clock* m_clock;
  access_point_credentials_t m_credentials;
  parser_t m_parser{};

  sjsu::arm::mission_control::mc_commands m_commands{};

  const std::span<hal::byte> m_buffer;
};
}  // namespace sjsu::arm