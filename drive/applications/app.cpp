#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>
#include <libhal-lpc40xx/can.hpp>
#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../implementation/command_lerper.hpp"
#include "../implementation/mission_control_handler.hpp"
#include "../implementation/mode_select.hpp"
#include "../implementation/mode_switcher.hpp"
#include "../implementation/rules_engine.hpp"
#include "../implementation/tri_wheel_router.hpp"

#include "../hardware_map.hpp"

#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>

#include <string_view>

#include "common/util.hpp"

hal::status application(drive::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& esp = *p_map.esp;
  auto& terminal = *p_map.terminal;
  auto& clock = *p_map.steady_clock;
  auto& magnet0 = *p_map.in_pin0;
  auto& magnet1 = *p_map.in_pin1;
  auto& magnet2 = *p_map.in_pin2;
  auto& can = *p_map.can;

  std::array<hal::byte, 8192> buffer{};
  static std::string_view get_request = "";

  HAL_CHECK(hal::write(terminal, "Starting program...\n"));

  auto wifi_result = hal::esp8266::at::wlan_client::create(
    esp, "Corey", "0123456789", HAL_CHECK(hal::create_timeout(clock, 10s)));

  if (!wifi_result) {
    HAL_CHECK(hal::write(terminal, "Failed to create wifi client!\n"));
    return wifi_result.error();
  }

  auto wifi = wifi_result.value();

  auto socket_result =
    hal::esp8266::at::socket::create(wifi,
                                     HAL_CHECK(hal::create_timeout(clock, 1s)),
                                     {
                                       .type = hal::socket::type::tcp,
                                       .domain = "13.56.207.97",
                                       .port = "5000",
                                     });
  // this is for the web server hosted by nate: http://13.56.207.97:5000/drive
  if (!socket_result) {
    HAL_CHECK(hal::write(terminal, "TCP Socket couldn't be established\n"));
    return socket_result.error();
  }

  auto socket = std::move(socket_result.value());
  HAL_CHECK(hal::write(terminal, "Server found"));
  auto can_router = hal::can_router::create(can).value();

  auto left_steer_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, clock, 6.0, 0x141));
  auto left_hub_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, clock, 15.0, 0x142));
  auto back_steer_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, clock, 6.0, 0x145));
  auto back_hub_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, clock, 15.0, 0x146));
  auto right_steer_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, clock, 6.0, 0x143));
  auto right_hub_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, clock, 15.0, 0x144));

  Drive::tri_wheel_router::leg right(
    right_steer_motor, right_hub_motor, magnet0);
  Drive::tri_wheel_router::leg left(left_steer_motor, left_hub_motor, magnet2);
  Drive::tri_wheel_router::leg back(back_steer_motor, back_hub_motor, magnet1);

  Drive::tri_wheel_router tri_wheel{ right, left, back };
  Drive::drive_commands commands;
  Drive::motor_feedback motor_speeds;
  Drive::tri_wheel_router_arguments arguments;

  Drive::mode_switch mode_switcher;
  Drive::command_lerper lerp;

  HAL_CHECK(hal::delay(clock, 1000ms));
  tri_wheel.home(clock);
  HAL_CHECK(hal::delay(clock, 1000ms));
  HAL_CHECK(hal::write(terminal, "Starting control loop..."));

  while (true) {
    buffer.fill('.');
    get_request = "GET /drive HTTP/1.1\r\n"
                  "Host: 13.56.207.97:5000/\r\n"
                  "\r\n";

    auto write_result = socket.write(
      hal::as_bytes(get_request), HAL_CHECK(hal::create_timeout(clock, 500ms)));
    if (!write_result) {
      continue;
    }

    HAL_CHECK(hal::delay(clock, 100ms));

    auto received = HAL_CHECK(socket.read(buffer)).data;

    auto result = to_string_view(received);

    auto start = result.find('{');
    auto end = result.find('}');
    auto json = result.substr(start, end - start + 1);

    HAL_CHECK(hal::write(terminal, json));
    HAL_CHECK(hal::write(terminal, "\r\n\n"));

    auto new_commands = Drive::parse_mission_control_data(json, terminal);
    if (new_commands) {
      commands = new_commands.value();
    }
    commands = Drive::validate_commands(commands);
    commands = mode_switcher.switch_steer_mode(
      commands, arguments, motor_speeds, terminal);
    commands.speed = lerp.lerp(commands.speed);

    // commands.print();
    arguments = Drive::select_mode(commands);
    HAL_CHECK(tri_wheel.move(arguments, clock));

    motor_speeds = HAL_CHECK(tri_wheel.get_motor_feedback(clock));
    // TODO(#issue_number): Use time out timer
    // TODO(#): Fix up parsing
  }

  return hal::success();
}
