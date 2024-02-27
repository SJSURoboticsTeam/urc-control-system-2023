#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>

#include "../implementations/joint_router.hpp"
#include "../implementations/rules_engine.hpp"
#include "../implementations/speed_control.hpp"

#include "application.hpp"
#include "../platform-implementations/constants.hpp"
namespace sjsu::arm {

hal::status application(sjsu::arm::application_framework& p_framework)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& feedback_getter = p_framework.feedback;
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

  auto& mission_control = *(p_framework.mc);
  auto loop_count = 0;
  auto& end_effector = *p_framework.end_effector;

  // mission control init should go here, if anything is needed

  std::string_view json;

  joint_router arm(rotunda_servo,
                   shoulder_servo,
                   elbow_servo,
                   left_wrist_servo,
                   right_wrist_servo,
                   end_effector,
                   rotunda_accelerometer,
                   shoulder_accelerometer,
                   elbow_accelerometer,
                   wrist_accelerometer);

  sjsu::arm::mission_control::mc_commands commands;
  speed_control speed_control;

  HAL_CHECK(hal::write(terminal, "Starting control loop..."));
  hal::delay(clock, 1000ms);

  arm_state current_state;
  arm_state target_state;
  arm_state d_state;
  arm_state kP;
  kP.rotunda = 1;
  kP.end_effector = 1;
  kP.elbow = 1;
  kP.wrist_pitch = 1;
  kP.wrist_roll = 1;
  kP.shoulder = 1;

  arm_state max_speed;
  max_speed.rotunda = max_speed.wrist_roll = max_speed.wrist_pitch = max_speed.elbow = max_speed.shoulder = max_speed.end_effector = 360;

  float next_update = static_cast<float>(clock.uptime().ticks) / clock.frequency().operating_frequency + 5;
  float then = static_cast<float>(clock.uptime().ticks) / clock.frequency().operating_frequency;
  while (true) {
    // Calculate time since last frame. Use this for physics.
    float now = static_cast<float>(clock.uptime().ticks) / clock.frequency().operating_frequency;
    float dt = now - then;
    then = now;

    if(next_update < now) {
      auto timeout = hal::create_timeout(clock, 1s);
      
      auto feedback = feedback_getter.get_feedback();
      feedback.dt = dt;
      mission_control.set_feedback(feedback);

      commands = mission_control.get_command(timeout).value();
      
      target_state.elbow = static_cast<float>(commands.elbow_angle) / precision_multiplier;
      target_state.shoulder = static_cast<float>(commands.shoulder_angle) / precision_multiplier;
      target_state.rotunda = static_cast<float>(commands.rotunda_angle) / precision_multiplier;
      target_state.wrist_pitch = static_cast<float>(commands.wrist_pitch_angle) / precision_multiplier;
      target_state.wrist_roll = static_cast<float>(commands.wrist_roll_angle) / precision_multiplier;
      target_state.end_effector = static_cast<float>(commands.rr9_angle);
      // next_update = static_cast<float>(clock.uptime().ticks) / clock.frequency().operating_frequency + 0.1;
    }
    // commands = validate_commands(commands);
    // commands = speed_control.lerp(commands);
    // HAL_CHECK(hal::write(terminal, "\n\n"));
    // commands.print(&terminal);
    d_state.rotunda = (target_state.rotunda - current_state.rotunda) * kP.rotunda;
    d_state.rotunda = std::clamp(d_state.rotunda, -max_speed.rotunda, max_speed.rotunda);
    current_state.rotunda += d_state.rotunda * dt;

    d_state.elbow = (target_state.elbow - current_state.elbow) * kP.elbow;
    d_state.elbow = std::clamp(d_state.elbow, -max_speed.elbow, max_speed.elbow);
    current_state.elbow += d_state.elbow * dt;

    d_state.shoulder = (target_state.shoulder - current_state.shoulder) * kP.shoulder;
    d_state.shoulder = std::clamp(d_state.shoulder, -max_speed.shoulder, max_speed.shoulder);
    current_state.shoulder += d_state.shoulder * dt;
    
    d_state.wrist_pitch = (target_state.wrist_pitch - current_state.wrist_pitch) * kP.wrist_pitch;
    d_state.wrist_pitch = std::clamp(d_state.wrist_pitch, -max_speed.wrist_pitch, max_speed.wrist_pitch);
    current_state.wrist_pitch += d_state.wrist_pitch * dt;

    d_state.wrist_roll = (target_state.wrist_roll - current_state.wrist_roll) * kP.wrist_roll;
    d_state.wrist_roll = std::clamp(d_state.wrist_roll, -max_speed.wrist_roll, max_speed.wrist_roll);
    current_state.wrist_roll += d_state.wrist_roll * dt;

    current_state.end_effector = target_state.end_effector;
    end_effector.position(current_state.end_effector);

    arm.move(current_state);

    // hal::delay(clock, 8ms);
  }

  return hal::success();
}
}  // namespace sjsu::arm
