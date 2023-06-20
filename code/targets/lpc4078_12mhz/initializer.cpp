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

hal::result<sjsu::hardware_map> initialize_target()
{
  using namespace hal::literals;

  hal::cortex_m::initialize_data_section();
  hal::cortex_m::system_control::initialize_floating_point_unit();

  HAL_CHECK(hal::lpc40xx::clock::maximum(12.0_MHz));

  auto& clock = hal::lpc40xx::clock::get();
  auto cpu_frequency = clock.get_frequency(hal::lpc40xx::peripheral::cpu);
  static hal::cortex_m::dwt_counter counter(cpu_frequency);

  auto& uart0 = HAL_CHECK((hal::lpc40xx::uart::get<0, 64>(hal::serial::settings{
    .baud_rate = 38400,
  })));

  hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  auto& can = HAL_CHECK((hal::lpc40xx::can::get<2>(can_settings)));

  auto& in_pin0 = HAL_CHECK((hal::lpc40xx::input_pin::get<1, 15>()));
  auto& in_pin1 = HAL_CHECK((hal::lpc40xx::input_pin::get<1, 23>()));
  auto& in_pin2 = HAL_CHECK((hal::lpc40xx::input_pin::get<1, 22>()));

  auto& can_pin = HAL_CHECK((hal::lpc40xx::input_pin::get<1, 20>()));

  auto& pwm_1_6 = HAL_CHECK((hal::lpc40xx::pwm::get<1, 6>()));
  auto& pwm_1_5 = HAL_CHECK((hal::lpc40xx::pwm::get<1, 5>()));

  auto& adc_4 = HAL_CHECK(hal::lpc40xx::adc::get<4>());
  auto& adc_5 = HAL_CHECK(hal::lpc40xx::adc::get<5>());

  auto& uart1 =
    HAL_CHECK((hal::lpc40xx::uart::get<1, 8192>(hal::serial::settings{
      .baud_rate = 115200,
    })));

  auto& i2c = HAL_CHECK((hal::lpc40xx::i2c::get<2>(hal::i2c::settings{
    .clock_rate = 100.0_kHz,
  })));

  return sjsu::hardware_map{ .terminal = &uart0,
                             .can = &can,
                             .in_pin0 = &in_pin0,
                             .in_pin1 = &in_pin1,
                             .in_pin2 = &in_pin2,
                             .can_pin = &can_pin,
                             .pwm_1_6 = &pwm_1_6,
                             .pwm_1_5 = &pwm_1_5,
                             .adc_4 = &adc_4,
                             .adc_5 = &adc_5,
                             .esp = &uart1,
                             .i2c = &i2c,
                             .steady_clock = &counter,
                             .reset = []() {
                               hal::cortex_m::system_control::reset();
                             } };
}
