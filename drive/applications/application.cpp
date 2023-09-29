#include <libhal-util/steady_clock.hpp>

#include "../dto/motor_feedback.hpp"

#include "../implementations/rules_engine.hpp"
#include "../implementations/command_lerper.hpp"
#include "../implementations/mode_select.hpp"
#include "../implementations/mode_switcher.hpp"
#include "../implementations/tri_wheel_router.hpp"

#include "../platform-implementations/mission_control.hpp"
#include "application.hpp"

namespace sjsu::drive {

hal::status application(application_framework& p_framework)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& left_leg = *p_framework.legs[0];
  auto& right_leg = *p_framework.legs[1];
  auto& back_leg = *p_framework.legs[2];
  auto& mission_control = *(p_framework.mc);
  auto& terminal = *p_framework.terminal;
  auto& clock = *p_framework.clock;

  sjsu::drive::tri_wheel_router tri_wheel{back_leg, right_leg, left_leg};
  sjsu::drive::mission_control::mc_commands commands;
  sjsu::drive::motor_feedback motor_speeds;
  sjsu::drive::tri_wheel_router_arguments arguments;

  sjsu::drive::mode_switch mode_switcher;
  sjsu::drive::command_lerper lerp;

  hal::delay(clock, 1000ms);
  HAL_CHECK(hal::write(terminal, "Starting control loop..."));

  while (true) {
    motor_speeds = HAL_CHECK(tri_wheel.get_motor_feedback());
    auto timeout = hal::create_timeout(clock, 10s);

    commands = mission_control.get_command(timeout).value();
    commands = sjsu::drive::validate_commands(commands);
    commands = mode_switcher.switch_steer_mode(
      commands, arguments, motor_speeds, terminal);
    commands.speed = lerp.lerp(commands.speed);
    arguments = sjsu::drive::select_mode(commands);
    HAL_CHECK(tri_wheel.move(arguments, clock));
  }

  return hal::success();
}

}