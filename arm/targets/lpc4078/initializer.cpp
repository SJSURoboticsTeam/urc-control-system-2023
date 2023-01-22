#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40xx/constants.hpp>
#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-lpc40xx/output_pin.hpp>
#include <libhal-lpc40xx/system_controller.hpp>
#include <libhal-lpc40xx/uart.hpp>

#include <libhal-lpc40xx/can.hpp>
// #include <libhal-lpc40xx/i2c.hpp>

#include "../../hardware_map.hpp"

hal::result<drive::hardware_map> initialize_target()
{
  using namespace hal::literals;

  hal::cortex_m::initialize_data_section();
  hal::cortex_m::system_control::initialize_floating_point_unit();

  HAL_CHECK(hal::lpc40xx::clock::maximum(10.0_MHz));

  // Clock declaration
  auto& clock = hal::lpc40xx::clock::get();
  auto cpu_frequency = clock.get_frequency(hal::lpc40xx::peripheral::cpu);
  static hal::cortex_m::dwt_counter counter(cpu_frequency);

  auto& uart0 = HAL_CHECK((hal::lpc40xx::uart::get<0, 64>(hal::serial::settings{
    .baud_rate = 38400,
  })));
  hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  auto& can = HAL_CHECK((hal::lpc40xx::can::get<1>(can_settings)));

  // Get and initialize UART3 with a 8kB receive buffer
  auto& uart3 =
    HAL_CHECK((hal::lpc40xx::uart::get<3, 8192>(hal::serial::settings{
      .baud_rate = 115200,
    })));

  // auto& i2c = HAL_CHECK((hal::lpc40xx::i2c::get<0>()));

  return drive::hardware_map{ .terminal = &uart0,
                              .can = &can,
                              .esp = &uart3,
                              // .i2c = &i2c,
                              .steady_clock = &counter,
                              .reset = []() {
                                hal::cortex_m::system_control::reset();
                              } };
}
