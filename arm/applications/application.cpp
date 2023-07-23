#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>

#include "application.hpp"

hal::status application(application_framework& p_framework)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& terminal = *p_framework.terminal;
  auto& clock = *p_framework.steady_clock;

  // mission control init should go here, if anything is needed
  
  std::string_view json;

  arm::joint_router arm(rotunda_motor,
                        shoulder_motor,
                        elbow_motor,
                        left_wrist_motor,
                        right_wrist_motor,
                        pwm0);

  arm::mc_commands commands;
  arm::motors_feedback feedback;

  HAL_CHECK(hal::write(terminal, "Starting control loop..."));
  HAL_CHECK(hal::delay(clock, 1000ms));

  while (true) {

    commands = arm::validate_commands(commands);

    commands.print(terminal);

    arm.move(commands);
  }

  return hal::success();
}
