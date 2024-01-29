#include <string_view>

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40/can.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/input_pin.hpp>
#include <libhal-lpc40/uart.hpp>
#include <libhal-lpc40/clock.hpp>

#include <libhal-util/units.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-lpc40/input_pin.hpp>

#include "../platform-implementations/drc_speed_sensor.cpp"

#include "../applications/application.hpp"

#include <cmath>

namespace sjsu::drive {

hal::status initialize_processor(){
  
  hal::cortex_m::initialize_data_section();
  hal::cortex_m::initialize_floating_point_unit();
  return hal::success();
}

hal::result<application_framework> initialize_platform() 
{
  using namespace hal::literals;
  using namespace std::chrono_literals;
  
  // setting clock
  HAL_CHECK(hal::lpc40::clock::maximum(12.0_MHz));
  auto& clock = hal::lpc40::clock::get();
  auto cpu_frequency = clock.get_frequency(hal::lpc40::peripheral::cpu);
  static hal::cortex_m::dwt_counter counter(cpu_frequency);

  // Serial
  static std::array<hal::byte, 1024> recieve_buffer0{};
  static auto uart0 = HAL_CHECK((hal::lpc40::uart::get(0, recieve_buffer0, hal::serial::settings{
    .baud_rate = 38400,
  })));
  // servos, we need to init all of the mc_x motors then call make_servo 
  // in order to pass servos into the application
  static hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  static auto can = HAL_CHECK((hal::lpc40::can::get(2, can_settings)));

  static auto can_router = hal::can_router::create(can).value();
  // left leg

  // static auto left_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x141));
  // static auto left_leg_drc_servo = HAL_CHECK(hal::make_servo(left_leg_steer_drc, 5.0_rpm));
  // static auto left_leg_drc_steer_speed_sensor = HAL_CHECK(make_speed_sensor(left_leg_steer_drc));
  // auto left_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 22, hal::input_pin::settings{}));

  // static auto left_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x142));
  // static auto left_leg_drc_motor = HAL_CHECK(hal::make_motor(left_leg_hub_drc, 100.0_rpm));
  // static auto left_leg_drc_hub_speed_sensor = HAL_CHECK(make_speed_sensor(left_leg_hub_drc));
  
  // static auto left_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(left_leg_drc_servo, 0.0f));

  // static auto left_home = homing{&left_leg_drc_offset_servo, &left_leg_mag, false};
  // static auto left_leg_drc_speed_sensor = HAL_CHECK(print_speed_sensor::make_speed_sensor(uart0));
  // static auto left_leg_drc_offset_servo  = HAL_CHECK(print_servo::create(uart0));
  // static auto left_leg_drc_motor = HAL_CHECK(print_motor::create(uart0));


  // right leg
  // static auto right_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x143));
  // static auto right_leg_drc_servo = HAL_CHECK(hal::make_servo(right_leg_steer_drc, 5.0_rpm));
  // static auto right_leg_drc_steer_speed_sensor = HAL_CHECK(make_speed_sensor(right_leg_steer_drc));
  // auto right_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 15, hal::input_pin::settings{}));

  // static auto right_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x144));
  // static auto right_leg_drc_motor = HAL_CHECK(hal::make_motor(right_leg_hub_drc, 100.0_rpm));
  // static auto right_leg_drc_hub_speed_sensor = HAL_CHECK(make_speed_sensor(right_leg_hub_drc));

  // static auto right_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(right_leg_drc_servo, 0.0f));
  // static auto right_home = homing{&right_leg_drc_offset_servo, &right_leg_mag, false};
  // static auto right_leg_drc_speed_sensor = HAL_CHECK(print_speed_sensor::make_speed_sensor(uart0));
  // static auto right_leg_drc_offset_servo  = HAL_CHECK(print_servo::create(uart0));
  // static auto right_leg_drc_motor = HAL_CHECK(print_motor::create(uart0));


  // back leg
  // static auto back_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x145));
  // static auto back_leg_drc_servo = HAL_CHECK(hal::make_servo(back_leg_steer_drc, 5.0_rpm));
  // static auto back_leg_drc_steer_speed_sensor = HAL_CHECK(make_speed_sensor(back_leg_steer_drc));
  // auto back_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 23, hal::input_pin::settings{}));

  // static auto back_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 6.0, 0x146));
  // static auto back_leg_drc_motor = HAL_CHECK(hal::make_motor(back_leg_hub_drc, 100.0_rpm));
  // static auto back_leg_drc_hub_speed_sensor = HAL_CHECK(make_speed_sensor(back_leg_hub_drc));

  // static auto back_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(back_leg_drc_servo, 0.0f));
  // static auto back_home = homing{&back_leg_drc_offset_servo, &back_leg_mag, false};

  // static auto back_leg_drc_speed_sensor = HAL_CHECK(print_speed_sensor::make_speed_sensor(uart0));
  // static auto back_leg_drc_offset_servo  = HAL_CHECK(print_servo::create(uart0));
  // static auto back_leg_drc_motor = HAL_CHECK(print_motor::create(uart0));


  // extra leg
  // static auto extra_leg_steer_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x147));
  // static auto extra_leg_drc_servo = HAL_CHECK(hal::make_servo(extra_leg_steer_drc, 5.0_rpm));
  // auto extra_leg_mag = HAL_CHECK(hal::lpc40::input_pin::get(1, 23, hal::input_pin::settings{}));

  // static auto extra_leg_hub_drc = HAL_CHECK(hal::rmd::drc::create(can_router, counter, 8.0, 0x148));
  // static auto extra_leg_drc_motor = HAL_CHECK(hal::make_motor(extra_leg_hub_drc, 100.0_rpm));
  // static auto extra_leg_drc_speed_sensor = HAL_CHECK(make_speed_sensor(extra_leg_hub_drc));
  
  // static auto extra_leg_drc_offset_servo = HAL_CHECK(offset_servo::create(extra_leg_drc_servo, 0.0f));
  // static auto extra_home = homing{&extra_leg_drc_offset_servo, &extra_leg_mag, false};
  const size_t number_of_legs = 3;

  // static std::array<homing*, number_of_legs> homing_structs = {
  //       &left_home,
  //       &right_home,
  //       &back_home,
  //       // &extra_home,
  //   };

  // HAL_CHECK(home(homing_structs, counter, &uart0));
  // hal::print<100>(uart0, "right offset: %f", right_home.servo->get_offset());
  // hal::print<100>(uart0, "left offset: %f", left_home.servo->get_offset());
  // hal::print<100>(uart0, "back offset: %f", back_home.servo->get_offset());
  // static leg left_leg{.steer = &left_leg_drc_offset_servo, 
  //             .propulsion = &left_leg_drc_motor,
  //             .steer_speed_sensor = &left_leg_drc_steer_speed_sensor,
  //             .propulsion_speed_sensor = &left_leg_drc_hub_speed_sensor,
  //             };

  // static leg right_leg{.steer = &right_leg_drc_offset_servo, 
  //             .propulsion = &right_leg_drc_motor,
  //             .steer_speed_sensor = &right_leg_drc_steer_speed_sensor,
  //             .propulsion_speed_sensor = &right_leg_drc_hub_speed_sensor,
  //             };

  // static leg back_leg{.steer = &back_leg_drc_offset_servo, 
  //             .propulsion = &back_leg_drc_motor,
  //             .steer_speed_sensor = &back_leg_drc_steer_speed_sensor,
  //             .propulsion_speed_sensor = &back_leg_drc_hub_speed_sensor
  //             };
  // static leg extra_leg{.steer = &extra_leg_drc_offset_servo, 
  //             .propulsion = &extra_leg_drc_motor,
  //             .spd_sensor = &extra_leg_drc_speed_sensor,
  //             .propulsion_speed_sensor = &
  //             };
// 


  static std::array<vector2, number_of_legs> wheel_locations = {
      vector2::from_bearing(1, -30 * std::numbers::pi / 180),
      vector2::from_bearing(1, 30 * std::numbers::pi / 180),
      vector2::from_bearing(1, std::numbers::pi),
      // NO IDEA WHERE THE 4th LEG IS
  };

  static std::array<wheel_setting, number_of_legs> wheel_settings;

  static ackermann_steering steering(wheel_locations, wheel_settings, 2, 2); // WE ARE UNABLE TO USE MAXIMUM ANGULAR SPEED UNLESS WE HAVE THE CORRECT SCALE FACTORS SET. 

  // static std::array<leg*, number_of_legs> legs = {
  //   &left_leg, 
  //   &right_leg, 
  //   &back_leg,
  //   // extra_leg,
  // };

  return application_framework{
                              .steering = &steering,
                              
                              .terminal = &uart0,
                              .clock = &counter,
                              .reset = []() { hal::cortex_m::reset(); },
                              };
}

}
