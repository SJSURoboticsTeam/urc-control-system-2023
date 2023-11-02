#include <libhal-util/serial.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../implementations/joint_router.hpp"
#include "../implementations/speed_control.hpp"
#include "../implementations/rules_engine.hpp"


#include "application.hpp"

namespace sjsu::arm {

hal::status application(sjsu::arm::application_framework& p_framework)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& terminal = *p_framework.terminal;
  auto& clock = *p_framework.clock;
  auto& rotunda_servo = *p_framework.rotunda_servo;
  auto& shoulder_servo = *p_framework.shoulder_servo;
  auto& elbow_servo = *p_framework.elbow_servo;
  auto& left_wrist_servo = *p_framework.left_wrist_servo;
  auto& right_wrist_servo = *p_framework.right_wrist_servo;
  auto& mission_control = *(p_framework.mc);
  auto loop_count = 0;
  // auto& end_effector = *p_framework.end_effector;

  // mission control init should go here, if anything is needed
  
  std::string_view json;

  joint_router arm(rotunda_servo,
                        shoulder_servo,
                        elbow_servo,
                        left_wrist_servo,
                        right_wrist_servo);

  sjsu::arm::mission_control::mc_commands commands;
  speed_control speed_control;

  HAL_CHECK(hal::write(terminal, "Starting control loop..."));
  hal::delay(clock, 1000ms);

  while (true) {
    if(loop_count==10) {
      auto timeout = hal::create_timeout(clock, 60ms);
      commands = mission_control.get_command(timeout).value();
      loop_count=0;
    }
    loop_count++;
    commands = validate_commands(commands);
    // commands = speed_control.lerp(commands);
    // HAL_CHECK(hal::write(terminal, "\n\n"));
    commands.print(&terminal);
    arm.move(commands);
    hal::delay(clock, 8ms);
  }

  return hal::success();
}
}
