#include <libhal-util/steady_clock.hpp>

#include "../include/drive_debug.hpp"
#include "../include/drive_configuration_updater.hpp"
#include "../include/wheel_router.hpp"
#include "../include/rules_engine.hpp"

#include "application.hpp"

namespace sjsu::drive {

hal::status application(application_framework& p_framework)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto legs = p_framework.legs;
  auto& steering = *p_framework.steering;
  auto& mission_control = *p_framework.mc;
  auto& terminal = *p_framework.terminal;
  auto& clock = *p_framework.clock;
  
  // Test Steering math for debug purposes.
  hal::print(terminal, "Testing Steering Math...\n");
  test_steering_math(terminal, steering);


  drive_configuration_updater configuration_updater;
  drive_configuration kP, max_delta;
  // These control how sensitive the system is to changes in the target.
  kP.steering_angle = 1;
  kP.wheel_heading = 10;
  kP.wheel_speed = 20;
  // These control how fast the system can react to changes in the target.
  max_delta.steering_angle = 40;
  max_delta.wheel_heading = 360;
  max_delta.wheel_speed = 50;

  configuration_updater.set_kP(kP);
  configuration_updater.set_max_delta(max_delta);

  // Supports different leg configurations
  wheel_router wheels(legs);

  hal::delay(clock, 1000ms);
  HAL_CHECK(hal::write(terminal, "Starting control loop..."));

  float next_update = static_cast<float>(clock.uptime().ticks) / clock.frequency().operating_frequency + 5;
  float then = static_cast<float>(clock.uptime().ticks) / clock.frequency().operating_frequency;
  while (true) {
    // Calculate time since last frame. Use this for physics.
    float now = static_cast<float>(clock.uptime().ticks) / clock.frequency().operating_frequency;
    float dt = now - then;
    then = now;

    if (next_update < now) {
      // Time out in 10 ms
      auto timeout = hal::create_timeout(clock, 10ms);
      auto commands = mission_control.get_command(timeout).value();

      // Create a new target and set the updater to go to the new target
      drive_configuration target;
      target.steering_angle = commands.steering_angle;
      target.wheel_heading = commands.wheel_heading;
      target.wheel_speed = commands.wheel_speed;

      // Validate the target
      target = validate_configuration(target);
      
      configuration_updater.set_target(target);

      // Next update from mission control in 100 ms (0.1 s)
      next_update = now + 0.1;
    }

    // Update the configuration
    configuration_updater.update(dt);
    // Get the current configuration
    drive_configuration current_configuration = configuration_updater.get_current();
    
    // Calculate the turning radius
    float turning_radius = 1 / std::tan(current_configuration.steering_angle * std::numbers::pi / 180);
    // Calculate the current wheel settings.
    auto wheel_settings = steering.calculate_wheel_settings(turning_radius, current_configuration.wheel_heading, current_configuration.wheel_speed / 100);

    // Move all the wheels
    HAL_CHECK(wheels.move(wheel_settings));
  }

  return hal::success();
}

}  // namespace sjsu::drive
