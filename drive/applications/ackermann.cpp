#include <libhal-util/steady_clock.hpp>
#include <libhal-util/serial.hpp>
#include "application.hpp"
// #include "../platform-implementations/serial_frame.hpp"
namespace sjsu::drive {

void print_wheel_settings(hal::serial& serial, std::span<wheel_setting> settings) {
  hal::print<1024>(serial, "|FL: %-5.1f°, %-5.1f rpm | FR: %-5.1f°, %-5.1f rpm | B: %-5.1f°, %-5.1f rpm |\n", 
    settings[0].angle,
    settings[0].wheel_speed, 
    settings[1].angle,
    settings[1].wheel_speed, 
    settings[2].angle,
    settings[2].wheel_speed);
}

hal::status set_wheel_state(std::span<leg*> legs, std::span<wheel_setting> settings) {
  HAL_CHECK(legs[0]->steer->position(wheel_settings[0].angle));
  HAL_CHECK(legs[1]->steer->position(wheel_settings[1].angle));
  HAL_CHECK(legs[2]->steer->position(wheel_settings[2].angle));
  HAL_CHECK(legs[0]->propulsion->power(-wheel_settings[0].wheel_speed));
  HAL_CHECK(legs[1]->propulsion->power(wheel_settings[1].wheel_speed));
  HAL_CHECK(legs[2]->propulsion->power(wheel_settings[2].wheel_speed));
}

void test_steering_math(hal::serial& serial, ackermann_steering steering) {
  wheel_settings = steering.calculate_wheel_settings(std::numeric_limits<float>::infinity(), 20, 1);
  print_wheel_settings(terminal, wheel_setting);

  wheel_settings = steering.calculate_wheel_settings(std::numeric_limits<float>::infinity(), -120, 1);
  print_wheel_settings(terminal, wheel_setting);
  
  wheel_settings = steering.calculate_wheel_settings(0, 0, 1);
  print_wheel_settings(terminal, wheel_setting);
  
  wheel_settings = steering.calculate_wheel_settings(1/ std::tan(60 * std::numbers::pi / 180), 0, 0.1);
  print_wheel_settings(terminal, wheel_setting);
  
  wheel_settings = steering.calculate_wheel_settings(1/ std::tan(35.4 * std::numbers::pi / 180), 44.7, 0.5);
  print_wheel_settings(terminal, wheel_setting);
}


void debug_stop(hal::serial& serial) {
  hal::print(serial, "DEBUG STOPPED. REMOVE THIS IF NOT DEBUGGING\n");
  while(true);
}


hal::status application(application_framework& p_framework)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& legs = p_framework.legs;
  auto& steering = *p_framework.steering;
  auto& terminal = *p_framework.terminal;
  auto& clock = *p_framework.clock;
  auto& mission_control = *p_framework.mc;
  // auto loop_count = 0;


  // Test Steering math for debug purposes.
  hal::print(serial, "Testing Steering Math...\n");
  test_steering_math(serial, steering);

  // Stop here to check the steering math.
  // debug_stop(serial);
  

  HAL_CHECK(hal::write(terminal, "Starting control loop...\n"));
  
  float kP_steering_angle = 0.1;
  float kP_wheel_heading = 0.1;
  float kP_wheel_speed = 0.1;

  float max_d_steering_angle = 20;
  float max_d_wheel_heading = 30;
  float max_d_wheel_speed = 10;

  float current_steering_angle = 0.0;
  float current_wheel_heading = 0.0;
  float current_wheel_speed = 0.0;

  float target_steering_angle = 0.0;
  float target_wheel_heading = 0.0;
  float target_wheel_speed = 0.0;

  int loop_count = 0;
  int then = clock.uptime();
  while (true) {
    // Calculate time since last frame. Use this for physics.
    int now = clock.uptime();
    float dt = (now - then) / clock.frequency();
    then = now;
    
    // A steering configuration is defined by 2 numbers: Turning Radius and Turning Heading.
    // The turning radius defines how fast the rover will change its heading. 
    //    It will turn the fastest at a radius 0 and it will travel straight
    //    at a radius infinity.
    // Note: it might be more usefult to define turning radius as a "steering angle", where
    //    turning_radius = 1 / tan(steering_angle)
    //    This way 0 degrees of steeering angle corresponds to straight and 90 degrees
    //    is full turn (spin).
    // Turning heading is the direction the rover will move relative to the rover's forward.
    // In drive mode, turning heading is always 0 degrees (forward)
    // In translate mode, the turning radius is always infinite (straight) and the turning heading changes to the translate angle.
    // In spin mode, turning heading does not matter (it will probably be 0) and turning radius is 0.

    // Note: Due to signed 0 in floating point, A turning radius of +0 is different from a turning radius of -0.
    //    +0 will be turning in one direction, while -0 will turn in a different direction
    //    Spin mode will probably use one of these directions, and simply reverse wheel speeds.
    //    But this is useful for moving from drive mode to spin mode, since the rover spinning with the wheel
    //    facing the correct direction.

    // Note: Steering angles beyond 90 and -90 degrees are possible, and will result in the rover beginning to move "in reverse"
    //    The steering is completely continuous, so moving from 0 -> 90 -> 180 turning angles will result in the rover
    //    correctly moving from forwards to spin to reverse.

    if(loop_count >= 10) {
      auto timeout = hal::create_timeout(clock, 1s);
      auto commands = mission_control.get_command(timeout).value();
      
      loop_count=0;
    }
    loop_count ++;

    float d_steering_angle = (target_steering_angle - current_steering_angle) * kP_steering_angle;
    float d_wheel_heading = (target_wheel_heading - current_wheel_heading) * kP_wheel_heading;
    float d_wheel_speed = (target_wheel_speed - current_wheel_speed) * kP_wheel_speed;

    d_steering_angle = std::clamp(d_steering_angle, -max_d_steering_angle, max_d_steering_angle);
    d_wheel_heading = std::clamp(d_wheel_heading, -max_d_wheel_heading, max_d_wheel_heading);
    d_wheel_speed = std::clamp(d_wheel_speed, -max_d_wheel_speed, max_d_wheel_speed);

    current_steering_angle += d_steering_angle * dt;
    current_wheel_heading += d_wheel_heading * dt;
    current_wheel_speed += max_d_wheel_speed * dt;


    float turning_radius = 1 / std::tan(current_steering_angle * std::numbers::pi / 180);
    auto wheel_settings = steering.calculate_wheel_settings(turning_radius, current_wheel_heading, current_wheel_heading);
    print_wheel_settings(terminal, wheel_setting);

    HAL_CHECK(legs[0]->steer->position(wheel_settings[0].angle));
    HAL_CHECK(legs[1]->steer->position(wheel_settings[1].angle));
    HAL_CHECK(legs[2]->steer->position(wheel_settings[2].angle));

    hal::delay(clock, 1ms);
  }



  // frame_parser parser;  

  // while (true) {
    
  //   std::array<hal::byte, 64> buffer;
  //   auto read_result = HAL_CHECK(terminal.read(buffer));

  //   for(auto i = read_result.data.begin(); i != read_result.data.end(); i++) {
  //     bool frame_finish = parser.process_byte(*i);
  //     if(frame_finish) {
  //       auto frame = parser.get_frame();
  //       // Do frame parsing.
  //       hal::print<32>(terminal, "Recieved frame: %s\n", frame);
  //     }
  //   }
  //   // Print message
  //   hal::print(terminal, "Hello, World\n");

  //   // Toggle LED
  //   // HAL_CHECK(led.level(true));
  //   hal::delay(clock, 500ms);

  //   // HAL_CHECK(led.level(false));
  //   // hal::delay(clock, 500ms);
  // }

  return hal::success();
}

}
