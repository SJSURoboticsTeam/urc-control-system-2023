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
#include <cmath>

#include "src/util.hpp"

hal::status application(drive::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& esp = *p_map.esp;
  auto& terminal = *p_map.terminal;
  auto& counter = *p_map.steady_clock; 
  auto& magnet0 = *p_map.in_pin0;
  auto& magnet1 = *p_map.in_pin1;
  auto& magnet2 = *p_map.in_pin2;       
  auto& can = *p_map.can;        

  std::array<hal::byte, 8192> buffer{};
  static std::string_view get_request = "";

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
  std::string_view json;

  HAL_CHECK(hal::delay(counter, 1000ms));
  // tri_wheel.HomeLegs(terminal, counter);
  HAL_CHECK(hal::delay(counter, 1000ms));
  HAL_CHECK(hal::write(terminal, "Starting control loop..."));

  while (true) {
    buffer.fill('.');

    HAL_CHECK(hal::delay(counter, 100ms));
    auto received = HAL_CHECK(terminal.read(buffer)).data;
    auto result = to_string_view(received);
    hal::print<200>(terminal, "%.*s", static_cast<int>(result.size()), result.data());
    auto start = result.find('{');
    auto end = result.find('}');
    if(std::isnan(end)) {
      json = result.substr(start, end - start + 1);
    } 
    else {
      json = std::string_view("no result");
    }
    

    hal::print<200>(terminal, "%.*s", static_cast<int>(json.size()), json.data());
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