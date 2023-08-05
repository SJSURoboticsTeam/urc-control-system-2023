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

  auto& left_leg = p_framework.legs[0];
  auto& right_leg = p_framework.legs[1];
  auto& back_leg = p_framework.legs[2];
  auto& mission_control = *p_framework.mc;
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
  auto timeout = hal::create_timeout(clock, 500ms);

  commands = HAL_CHECK(mission_control.get_command(timeout));
  while (true) {
    // hal::print(terminal, "validating commands\n");
    commands = sjsu::drive::validate_commands(commands);
    // hal::print(terminal, "switching steer modes\n");
    commands = mode_switcher.switch_steer_mode(
      commands, arguments, motor_speeds, terminal);
    // hal::print(terminal, "Lerping commands\n");
    commands.speed = lerp.lerp(commands.speed);

// hal::print(terminal, "selecting modes\n");
    // commands.print();
    arguments = sjsu::drive::select_mode(commands);
    // hal::print(terminal, "Moving legs\n");
    HAL_CHECK(tri_wheel.move(arguments, clock));

    // TODO(#issue_number): Use time out timer
  }

  return hal::success();
}

}