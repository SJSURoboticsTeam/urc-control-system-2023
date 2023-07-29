#include <libhal-util/serial.hpp>
#include <libhal/steady_clock.hpp>

#include "application.hpp"

hal::status application(application_framework& p_framework)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& terminal = *p_framework.terminal;
  auto& clock = *p_framework.steady_clock;
  auto& rotunda_servo = *rotunda_servo;
  auto& shoulder_servo = *shoulder_servo;
  auto& elbow_servo = *elbow_servo;
  auto& left_wrist_servo = *left_wrist_servo;
  auto& right_wrist_servo = *right_wrist_servo;
  auto& end_effector = *end_effector;

  // mission control init should go here, if anything is needed
  
  std::string_view json;

  sjsu::arm::joint_router arm(rotunda_servo,
                        shoulder_servo,
                        elbow_servo,
                        left_wrist_servo,
                        right_wrist_servo,
                        end_effector);

  sjsu::arm::mc_commands commands;
  sjsu::arm::motors_feedback feedback;
  sjsu::arm::speed_control speed_control;

  HAL_CHECK(hal::write(terminal, "Starting control loop..."));
  HAL_CHECK(hal::delay(clock, 1000ms));

  while (true) {

    commands = sjsu::arm::validate_commands(commands);

    commands = speed_control.lerp(commands)

    commands.print(terminal);

    arm.move(commands);
  }

  return hal::success();
}
