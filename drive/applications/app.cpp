#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>
#include <libhal-lpc40xx/can.hpp>
#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../implementation/command-lerper.hpp"
#include "../implementation/mission-control-handler.hpp"
#include "../implementation/mode-select.hpp"
#include "../implementation/mode-switcher.hpp"
#include "../implementation/rules-engine.hpp"
#include "../implementation/tri-wheel-router.hpp"

#include "../hardware_map.hpp"

#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>

#include <string>
#include <string_view>

#include "src/util.hpp"

hal::status application(drive::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& esp = *p_map.esp;
  auto& terminal = *p_map.terminal;
  auto& counter = *p_map.steady_clock;  // check
  auto& magnet0 = *p_map.in_pin0;
  auto& magnet1 = *p_map.in_pin1;
  auto& magnet2 = *p_map.in_pin2;  //
  auto& can = *p_map.can;          // check

  std::array<hal::byte, 8192> buffer{};
  static std::string_view get_request = "";

  HAL_CHECK(hal::write(terminal, "Starting program...\n"));

  auto wifi_result = hal::esp8266::at::wlan_client::create(
    esp,
    "SJSU Robotics 2.4GHz",
    "R0Bot1cs3250",
    HAL_CHECK(hal::create_timeout(counter, 10s)));

  if (!wifi_result) {
    HAL_CHECK(hal::write(terminal, "Failed to create wifi client!\n"));
    return wifi_result.error();
  }

  auto wifi = wifi_result.value();

  auto socket_result = hal::esp8266::at::socket::create(
    wifi,
    HAL_CHECK(hal::create_timeout(counter, 1s)),
    {
      .type = hal::socket::type::tcp,
      .domain = "192.168.1.110",
      .port = "5000",
    });

  if (!socket_result) {
    HAL_CHECK(hal::write(terminal, "TCP Socket couldn't be established\n"));
    return socket_result.error();
  }

  auto socket = std::move(socket_result.value());

  auto can_router = hal::can_router::create(can).value();

  auto left_steer_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, 6.0, 0x141));
  auto left_hub_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, 15.0, 0x142));
  auto back_steer_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, 6.0, 0x145));
  auto back_hub_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, 15.0, 0x146));
  auto right_steer_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, 6.0, 0x143));
  auto right_hub_motor =
    HAL_CHECK(hal::rmd::drc::create(can_router, 15.0, 0x144));

  Drive::TriWheelRouter::leg right(right_steer_motor, right_hub_motor, magnet2);
  Drive::TriWheelRouter::leg left(left_steer_motor, left_hub_motor, magnet1);
  Drive::TriWheelRouter::leg back(back_steer_motor, back_hub_motor, magnet0);

  Drive::TriWheelRouter tri_wheel{ right, left, back };
  Drive::MissionControlHandler mission_control;
  Drive::drive_commands commands;
  Drive::motor_feedback motor_speeds;
  Drive::tri_wheel_router_arguments arguments;

  Drive::RulesEngine rules_engine;
  Drive::ModeSwitch mode_switch;
  Drive::CommandLerper lerp;

  HAL_CHECK(hal::delay(counter, 1000ms));
  // tri_wheel.HomeLegs(terminal);
  HAL_CHECK(hal::delay(counter, 1000ms));
  HAL_CHECK(hal::write(terminal, "Starting control loop..."));

  while (true) {
    buffer.fill('.');
    get_request = "GET /drive" + get_rover_status() +
                  " HTTP/1.1\r\n"
                  "Host: 192.168.1.110:5000/\r\n"
                  "\r\n";

    auto write_result =
      socket.write(hal::as_bytes(get_request),
                   HAL_CHECK(hal::create_timeout(counter, 500ms)));
    if (!write_result) {
      continue;
    }

    HAL_CHECK(hal::delay(counter, 100ms));

    auto received = HAL_CHECK(socket.read(buffer)).data;

    auto result = to_string_view(received);

    auto start = result.find('{');
    auto end = result.find('}');
    auto json = result.substr(start, end - start + 1);

    HAL_CHECK(hal::write(terminal, json));
    HAL_CHECK(hal::write(terminal, "\r\n\n"));

    std::string json_string(json);
    commands =
      HAL_CHECK(mission_control.ParseMissionControlData(json_string, terminal));
    commands = rules_engine.ValidateCommands(commands);
    commands = mode_switch.SwitchSteerMode(commands, arguments, motor_speeds);
    commands = lerp.Lerp(commands);

    // commands.Print();
    arguments = Drive::ModeSelect::SelectMode(commands);
    arguments = tri_wheel.SetLegArguments(arguments);

    motor_speeds = HAL_CHECK(tri_wheel.GetMotorFeedback());
  }

  return hal::success();
}