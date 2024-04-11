#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/timeout.hpp>

#include "../implementations/joint_router.hpp"
#include "../implementations/rules_engine.hpp"
#include "../implementations/speed_control.hpp"

#include "application.hpp"

namespace sjsu::arm {

void application(sjsu::arm::application_framework& p_framework)
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

  auto& rotunda_accelerometer = *p_framework.rotunda_accelerometer;
  auto& shoulder_accelerometer = *p_framework.shoulder_accelerometer;
  auto& elbow_accelerometer = *p_framework.elbow_accelerometer;
  auto& wrist_accelerometer = *p_framework.wrist_accelerometer;

  [[maybe_unused]]auto& mission_control = *(p_framework.mc);
  [[maybe_unused]]auto loop_count = 0;
  // auto& end_effector = *p_framework.end_effector;

  // mission control init should go here, if anything is needed
  joint_router arm(rotunda_servo,
                   shoulder_servo,
                   elbow_servo,
                   left_wrist_servo,
                   right_wrist_servo,
                   rotunda_accelerometer,
                   shoulder_accelerometer,
                   elbow_accelerometer,
                   wrist_accelerometer);

  const sjsu::arm::mission_control::mc_commands commands1 = {
    .heartbeat_count = 0,
    .is_operational = 0,
    .speed = 1,
    .rotunda_angle = 0,
    .shoulder_angle = 0,
    .elbow_angle = 0,
    .wrist_pitch_angle = 0,
    .wrist_roll_angle = 0,
    .rr9_angle = 0
  };

  const sjsu::arm::mission_control::mc_commands commands2 = {
    .heartbeat_count = 0,
    .is_operational = 0,
    .speed = 1,
    .rotunda_angle = 60,
    .shoulder_angle = 45,
    .elbow_angle = 10,
    .wrist_pitch_angle = 0,
    .wrist_roll_angle = 0,
    .rr9_angle = 0
  };
  const sjsu::arm::mission_control::mc_commands commands3 = {
    .heartbeat_count = 0,
    .is_operational = 0,
    .speed = 1,
    .rotunda_angle = -60,
    .shoulder_angle = -20,
    .elbow_angle = 45,
    .wrist_pitch_angle = 0,
    .wrist_roll_angle = 0,
    .rr9_angle = 0
  };
  const sjsu::arm::mission_control::mc_commands commands4 = {
    .heartbeat_count = 0,
    .is_operational = 0,
    .speed = 1,
    .rotunda_angle = -60,
    .shoulder_angle = 60,
    .elbow_angle = 0,
    .wrist_pitch_angle = 0,
    .wrist_roll_angle = 0,
    .rr9_angle = 0
  };
  hal::print(terminal, "Starting control loop...");
  hal::delay(clock, 1000ms);

  while (true) {
    commands1.print(&terminal);
    arm.move(commands1);
    hal::delay(clock, 45s);
    commands2.print(&terminal);
    arm.move(commands2);
    hal::delay(clock, 45s);
    commands3.print(&terminal);
    arm.move(commands3);
    hal::delay(clock, 45s);
    commands4.print(&terminal);
    arm.move(commands4);
    hal::delay(clock, 45s);
  }
}  // namespace sjsu::arm
}  // namespace sjsu::arm
