#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40xx/can.hpp>
#include <libhal-lpc40xx/constants.hpp>
#include <libhal-lpc40xx/i2c.hpp>
#include <libhal-lpc40xx/input_pin.hpp>
#include <libhal-lpc40xx/output_pin.hpp>
#include <libhal-lpc40xx/system_controller.hpp>
#include <libhal-lpc40xx/uart.hpp>
#include <libhal-util/units.hpp>

#include "../../hardware_map.hpp"

hal::result<sjsu::hardware_map> initialize_target()
{
  using namespace hal::literals;

  hal::cortex_m::initialize_data_section();
  hal::cortex_m::system_control::initialize_floating_point_unit();

  HAL_CHECK(hal::lpc40xx::clock::maximum(10.0_MHz));

  auto& clock = hal::lpc40xx::clock::get();
  auto cpu_frequency = clock.get_frequency(hal::lpc40xx::peripheral::cpu);
  static hal::cortex_m::dwt_counter counter(cpu_frequency);

  auto& uart0 = HAL_CHECK((hal::lpc40xx::uart::get<0, 64>(hal::serial::settings{
    .baud_rate = 38400,
  })));

  hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  auto& can = HAL_CHECK((hal::lpc40xx::can::get<2>(can_settings)));

  auto& in0 = HAL_CHECK((hal::lpc40xx::input_pin::get<2, 1>()));
  auto& in1 = HAL_CHECK((hal::lpc40xx::input_pin::get<2, 2>()));
  auto& in2 = HAL_CHECK((hal::lpc40xx::input_pin::get<2, 0>()));

  auto& uart3 =
    HAL_CHECK((hal::lpc40xx::uart::get<3, 8192>(hal::serial::settings{
      .baud_rate = 115200,
    })));

  auto& i2c = HAL_CHECK((hal::lpc40xx::i2c::get<2>(hal::i2c::settings{
    .clock_rate = 100.0_kHz,
  })));

  return sjsu::hardware_map{ .terminal = &uart0,
                             .can = &can,
                             .in_pin0 = &in0,
                             .in_pin1 = &in1,
                             .in_pin2 = &in2,
                             .esp = &uart3,
                             .i2c = &i2c,
                             .steady_clock = &counter,
                             .reset = []() {
                               hal::cortex_m::system_control::reset();
                             } };
}
