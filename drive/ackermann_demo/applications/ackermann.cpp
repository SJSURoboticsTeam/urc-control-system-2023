#include <libhal-util/steady_clock.hpp>
#include <libhal-util/serial.hpp>
#include "application.hpp"

namespace sjsu::drive {

hal::status application(application_framework& p_framework)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& steering = *p_framework.steering;
  auto& terminal = *p_framework.terminal;
  auto& clock = *p_framework.clock;
  auto loop_count = 0;

//   sjsu::drive::tri_wheel_router tri_wheel{legs};
//   sjsu::drive::mission_control::mc_commands commands;
//   sjsu::drive::motor_feedback motor_speeds;
//   sjsu::drive::tri_wheel_router_arguments arguments{};

//   sjsu::drive::mode_switch mode_switcher;
//   sjsu::drive::command_lerper lerp;

  auto wheel_settings = steering.calculate_wheel_settings(std::numeric_limits<float>::infinity(), 0, 1);
  hal::print<1024>(terminal, "|FL: %-5.1f°, %-5.1f rpm | FR: %-5.1f°, %-5.1f rpm | B: %-5.1f°, %-5.1f rpm |\n", 
    wheel_settings[0].angle,
    wheel_settings[0].wheel_speed, 
    wheel_settings[1].angle,
    wheel_settings[1].wheel_speed, 
    wheel_settings[2].angle,
    wheel_settings[2].wheel_speed);
  wheel_settings = steering.calculate_wheel_settings(std::numeric_limits<float>::infinity(), 20, 1);
  hal::print<1024>(terminal, "|FL: %-5.1f°, %-5.1f rpm | FR: %-5.1f°, %-5.1f rpm | B: %-5.1f°, %-5.1f rpm |\n", 
    wheel_settings[0].angle,
    wheel_settings[0].wheel_speed, 
    wheel_settings[1].angle,
    wheel_settings[1].wheel_speed, 
    wheel_settings[2].angle,
    wheel_settings[2].wheel_speed);
  wheel_settings = steering.calculate_wheel_settings(std::numeric_limits<float>::infinity(), -120, 1);
  hal::print<1024>(terminal, "|FL: %-5.1f°, %-5.1f rpm | FR: %-5.1f°, %-5.1f rpm | B: %-5.1f°, %-5.1f rpm |\n", 
    wheel_settings[0].angle,
    wheel_settings[0].wheel_speed, 
    wheel_settings[1].angle,
    wheel_settings[1].wheel_speed, 
    wheel_settings[2].angle,
    wheel_settings[2].wheel_speed);
  wheel_settings = steering.calculate_wheel_settings(0, 0, 1);
  hal::print<1024>(terminal, "|FL: %-5.1f°, %-5.1f rpm | FR: %-5.1f°, %-5.1f rpm | B: %-5.1f°, %-5.1f rpm |\n", 
    wheel_settings[0].angle,
    wheel_settings[0].wheel_speed, 
    wheel_settings[1].angle,
    wheel_settings[1].wheel_speed, 
    wheel_settings[2].angle,
    wheel_settings[2].wheel_speed);
  wheel_settings = steering.calculate_wheel_settings(0, 20, 1);
  hal::print<1024>(terminal, "|FL: %-5.1f°, %-5.1f rpm | FR: %-5.1f°, %-5.1f rpm | B: %-5.1f°, %-5.1f rpm |\n", 
    wheel_settings[0].angle,
    wheel_settings[0].wheel_speed, 
    wheel_settings[1].angle,
    wheel_settings[1].wheel_speed, 
    wheel_settings[2].angle,
    wheel_settings[2].wheel_speed);
  wheel_settings = steering.calculate_wheel_settings(0, -120, 1);
  hal::print<1024>(terminal, "|FL: %-5.1f°, %-5.1f rpm | FR: %-5.1f°, %-5.1f rpm | B: %-5.1f°, %-5.1f rpm |\n", 
    wheel_settings[0].angle,
    wheel_settings[0].wheel_speed, 
    wheel_settings[1].angle,
    wheel_settings[1].wheel_speed, 
    wheel_settings[2].angle,
    wheel_settings[2].wheel_speed);
  
  hal::delay(clock, 20000ms);
  HAL_CHECK(hal::write(terminal, "Starting control loop..."));



  hal::degrees steering_angle = -30;
  hal::degrees target_steering_angle = 30;
  while (true) {
    // if(loop_count==10) {
    //   auto timeout = hal::create_timeout(clock, 1s);
    //   commands = mission_control.get_command(timeout).value();
    //   loop_count=0;
    // }
    // loop_count++;
    // motor_speeds = HAL_CHECK(tri_wheel.get_motor_feedback());
    
    // commands = sjsu::drive::validate_commands(commands);

    // commands = mode_switcher.switch_steer_mode(
    // commands, arguments, motor_speeds, terminal);
    // commands.speed = lerp.lerp(commands.speed);
    
    // arguments = sjsu::drive::select_mode(commands, leg_count);
    // HAL_CHECK(tri_wheel.move(arguments, clock));
    // hal::delay(clock, 8ms);
    float turning_radius = 1 / std::tan(steering_angle);
    auto wheel_settings = steering.calculate_wheel_settings(turning_radius, 0, 0);
    hal::print<1024>(terminal, "|FL: %-5.1f°, %-5.1f rpm | FR: %-5.1f°, %-5.1f rpm | B: %-5.1f°, %-5.1f rpm |\n", 
        wheel_settings[0].angle,
        wheel_settings[0].wheel_speed, 
        wheel_settings[1].angle,
        wheel_settings[1].wheel_speed, 
        wheel_settings[2].angle,
        wheel_settings[2].wheel_speed);
    
    steering_angle = std::lerp(steering_angle, target_steering_angle, 0.01);
    hal::delay(clock, 10ms);

    if(std::abs(steering_angle - target_steering_angle) < 1) {
        target_steering_angle *= -1;
    }
  }

  return hal::success();
}

}
