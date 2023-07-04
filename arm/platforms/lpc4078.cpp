#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40xx/adc.hpp>
#include <libhal-lpc40xx/can.hpp>
#include <libhal-lpc40xx/constants.hpp>
#include <libhal-lpc40xx/i2c.hpp>
#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-lpc40xx/output_pin.hpp>
#include <libhal-lpc40xx/pwm.hpp>
#include <libhal-lpc40xx/system_controller.hpp>
#include <libhal-lpc40xx/uart.hpp>
#include <libhal-util/units.hpp>

#include "../../hardware_map.hpp"

hal::status initialize_processor(){
  
  hal::cortex_m::initialize_data_section();
  hal::cortex_m::system_control::initialize_floating_point_unit();
  return hall::success()
}

hal::result<application_framework> initialize_platform() 
{
  using namespace hal::literals;

  HAL_CHECK(hal::lpc40xx::clock::maximum(12.0_MHz));

  auto& clock = hal::lpc40xx::clock::get();
  auto cpu_frequency = clock.get_frequency(hal::lpc40xx::peripheral::cpu);
  static hal::cortex_m::dwt_counter counter(cpu_frequency);

  auto& uart0 = HAL_CHECK((hal::lpc40xx::uart::get<0, 64>(hal::serial::settings{
    .baud_rate = 38400,
  })));


  // servos
  hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  auto& can = HAL_CHECK((hal::lpc40xx::can::get<2>(can_settings)));
  


  // homing pins
  auto& in_pin0 = HAL_CHECK((hal::lpc40xx::input_pin::get<1, 15>()));
  auto& in_pin1 = HAL_CHECK((hal::lpc40xx::input_pin::get<1, 23>()));
  auto& in_pin2 = HAL_CHECK((hal::lpc40xx::input_pin::get<1, 22>()));

  auto& uart1 =
    HAL_CHECK((hal::lpc40xx::uart::get<1, 8192>(hal::serial::settings{
      .baud_rate = 115200,
    })));

  return sjsu::hardware_map{ .terminal = &uart0,
                             .rotunda_servo = &
                             .steady_clock = &counter,
                             .reset = []() {
                               hal::cortex_m::system_control::reset();
                             } };
}
