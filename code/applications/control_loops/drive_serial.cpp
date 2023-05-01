
#include <libhal-lpc40xx/can.hpp>
#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../../drive/implementation/command_lerper.hpp"
#include "../../drive/implementation/mission_control_handler.hpp"
#include "../../drive/implementation/mode_select.hpp"
#include "../../drive/implementation/mode_switcher.hpp"
#include "../../drive/implementation/rules_engine.hpp"
#include "../../drive/implementation/tri_wheel_router.hpp"

#include "../../hardware_map.hpp"

#include <cmath>
#include <string>
#include <string_view>

#include "common/util.hpp"

hal::status application(sjsu::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& terminal = *p_map.terminal;
  auto& clock = *p_map.steady_clock;
  auto& magnet0 = *p_map.in_pin0;
  auto& magnet1 = *p_map.in_pin1;
  auto& magnet2 = *p_map.in_pin2;
  auto& can = *p_map.can;

  std::array<hal::byte, 8192> buffer{};

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
  std::string_view json{
    "{\"HB\":0,\"IO\":0,\"WO\":0,\"DM\":\"D\",\"CMD\":[0,0]}"
  };

  HAL_CHECK(hal::delay(clock, 1000ms));
  // tri_wheel.home(clock);
  HAL_CHECK(hal::delay(clock, 1000ms));
  HAL_CHECK(hal::write(terminal, "Starting control loop..."));

  while (true) {
    // serial
    auto received = HAL_CHECK(terminal.read(buffer)).data;
    auto result = to_string_view(received);
    auto start = result.find('{');
    auto end = result.find('}');

    if (start != std::string::npos && end != std::string::npos) {
      result = result.substr(start, end - start + 1);
      auto new_commands = Drive::parse_mission_control_data(json, terminal);
      if (new_commands) {
        commands = new_commands.value();
      }
      commands.print(terminal);
    }
    // end of serial
    commands = validate_commands(commands);
    // commands = mode_switcher.switch_steer_mode(
      // commands, arguments, motor_speeds, terminal);
    // commands.speed = lerp.lerp(commands.speed);

    commands.print(terminal);
    // arguments = select_mode(commands);
    // HAL_CHECK(tri_wheel.move(arguments, clock));

    // motor_speeds = HAL_CHECK(tri_wheel.get_motor_feedback(clock));
    HAL_CHECK(hal::delay(clock, 30ms));
  }

  return hal::success();
}