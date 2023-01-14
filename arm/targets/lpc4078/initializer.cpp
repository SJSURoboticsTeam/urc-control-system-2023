#include <libarmcortex/dwt_counter.hpp>
#include <libarmcortex/startup.hpp>
#include <libarmcortex/system_control.hpp>

#include <liblpc40xx/constants.hpp>
#include <liblpc40xx/input_pin.hpp>
#include <liblpc40xx/output_pin.hpp>
#include <liblpc40xx/system_controller.hpp>
#include <liblpc40xx/uart.hpp>

#include <liblpc40xx/can.hpp>

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

  auto& can = HAL_CHECK((hal::lpc40xx::can::get<1>(hal::can::settings{.baud_rate = 100})));

  auto& in0 = HAL_CHECK((hal::lpc40xx::input_pin::get<2, 1>()));
  auto& in1 = HAL_CHECK((hal::lpc40xx::input_pin::get<2, 2>()));
  auto& in2 = HAL_CHECK((hal::lpc40xx::input_pin::get<2, 0>()));

    // Get and initialize UART3 with a 8kB receive buffer
  auto& uart3 =
    HAL_CHECK((hal::lpc40xx::uart::get<3, 8192>(hal::serial::settings{
      .baud_rate = 115200,
    })));

  return drive::hardware_map{
    .terminal = &uart0,
    .can = &can,
    .in_pin0 = &in0,
    .in_pin1 = &in1,
    .in_pin2 = &in2,
    .esp = &uart3,
    .steady_clock = &counter,
    .reset = []() { hal::cortex_m::system_control::reset(); }
  };
}
