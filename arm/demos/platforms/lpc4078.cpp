#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40/can.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/input_pin.hpp>
#include <libhal-lpc40/uart.hpp>
#include <libhal-lpc40/i2c.hpp>

#include <libhal-rmd/mc_x.hpp>
// #include <libhal-rmd/mc_x_servo.hpp>
#include <libhal-util/units.hpp>
#include <libhal-pca/pca9685.hpp>
#include "../hardware_map.hpp"
// #include "../../platform-implementations/mission_control.hpp"
// #include "../../platform-implementations/helper.hpp"
// #include "../../platform-implementations/serial_mission_control.cpp"

#include <libhal-lpc40/clock.hpp>
#include <libhal-lpc40/pwm.hpp>
#include <libhal-soft/rc_servo.hpp>

namespace sjsu::arm {

hal::status initialize_processor()
{

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
  static auto uart0 = HAL_CHECK((hal::lpc40::uart::get(0,
                                                       recieve_buffer0,
                                                       hal::serial::settings{
                                                         .baud_rate = 38400,
                                                       })));
  hal::print(uart0, "hello\n");
  // servos, we need to init all of the mc_x motors then call make_servo
  // in order to pass servos into the application
  
  static auto i2c = HAL_CHECK(hal::lpc40::i2c::get(2)); //need to use pca here

  static auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c,0b100'0000)); 
  static auto pwm0 = pca9685.get_pwm_channel<3>(); 
  auto servo_settings = hal::soft::rc_servo::settings{
  .min_angle = 0.0_deg, //to be tested with end effector
  .max_angle = 180.0_deg, 
  .min_microseconds = 500,
  .max_microseconds = 2500,
};
  static auto end_effector_servo =
    HAL_CHECK(hal::soft::rc_servo::create(pwm0, servo_settings));
    // mission control object


  return application_framework{
    .end_effector = &end_effector_servo,
    // .pca = &pca9685,
    .terminal = &uart0,
    // .mc = &serial_mc,
    .clock = &counter,
    .reset = []() { hal::cortex_m::reset(); },
  };
}
}  // namespace sjsu::arm