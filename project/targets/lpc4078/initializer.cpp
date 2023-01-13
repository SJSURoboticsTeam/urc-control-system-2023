#include <libarmcortex/dwt_counter.hpp>
#include <libarmcortex/startup.hpp>
#include <libarmcortex/system_control.hpp>

#include <liblpc40xx/constants.hpp>
#include <liblpc40xx/input_pin.hpp>
#include <liblpc40xx/output_pin.hpp>
#include <liblpc40xx/system_controller.hpp>
#include <liblpc40xx/uart.hpp>

#include <libhal-util/can.hpp>

#include "../../hardware_map.hpp"

hal::result<starter::hardware_map> initialize_target()
{
  using namespace hal::literals;

  hal::cortex_m::initialize_data_section();
  hal::cortex_m::system_control::initialize_floating_point_unit();

  auto& uart0 = HAL_CHECK((hal::lpc40xx::uart::get<0, 64>(hal::serial::settings{
    .baud_rate = 38400,
  })));

  auto& can = HAL_CHECK(hal::liblpc40xx::can::get<1>());

    // Get and initialize UART3 with a 8kB receive buffer
  auto& uart3 =
    HAL_CHECK((hal::lpc40xx::uart::get<3, 8192>(hal::serial::settings{
      .baud_rate = 115200,
    })));

  return Arm::hardware_map{
    .terminal = &uart0,
    .can = &can,
    .in_pin0 = &in0,
    .in_pin1 = &in1,
    .in_pin2 = &in2,
    .esp = &uart3,
    .steady_clock = &counter,
    .reset = []() { hal::cortex_m::system_control::reset(); },
  };
}
