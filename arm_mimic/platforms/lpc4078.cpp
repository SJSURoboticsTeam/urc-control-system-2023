#include <string_view>

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40/can.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/input_pin.hpp>
#include <libhal-lpc40/uart.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-lpc40/i2c.hpp>

#include <libhal-util/units.hpp>
#include <libhal-lpc40/input_pin.hpp>
#include "applications/application.hpp"

namespace sjsu::arm_mimic {

hal::status initialize_processor(){
  
  hal::cortex_m::initialize_data_section();
  hal::cortex_m::initialize_floating_point_unit();
  return hal::success();
}

hal::result<application_framework> initialize_platform() 
{
  using namespace hal::literals;
  using namespace std::chrono_literals;
  
  auto& clock = hal::lpc40::clock::get();
  auto cpu_frequency = clock.get_frequency(hal::lpc40::peripheral::cpu);
  static hal::cortex_m::dwt_counter counter(cpu_frequency);

  // Get and initialize UART0 for UART based terminal logging
  // auto& uart0 = HAL_CHECK((hal::lpc40::uart::get<0, 64>(hal::serial::settings{
  //   .baud_rate = 38400,
  // })));
  static std::array<hal::byte, 64> recieve_buffer0{};
  static auto uart0 = HAL_CHECK((hal::lpc40::uart::get(0, 
  recieve_buffer0,
  hal::serial::settings{
    .baud_rate = 38400,
  })));

  static auto i2c2 = HAL_CHECK(hal::lpc40::i2c::get(2));

  return application_framework{
  .terminal = &uart0,
  .i2c2 = &i2c2,
  .steady_clock = &counter,
  .reset = []() { hal::cortex_m::reset(); },
  };
};
}
