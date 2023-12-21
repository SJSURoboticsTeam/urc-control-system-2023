#include <string_view>

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40xx/adc.hpp>
#include <libhal-lpc40/can.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/input_pin.hpp>
#include <libhal-lpc40xx/i2c.hpp>
#include <libhal-lpc40/output_pin.hpp>
#include <libhal-lpc40/pwm.hpp>
#include <libhal-lpc40xx/system_controller.hpp>
#include <libhal-lpc40/uart.hpp>

#include <libhal-util/units.hpp>

#include "../../applications/application.hpp"
#include "../../platform-implementations/helper.hpp"
#include "../../platform-implementations/pwm_relay.hpp"

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
  
  // Setting Clock
  HAL_CHECK(hal::lpc40::clock::maximum(12.0_MHz));
  auto& clock = hal::lpc40::clock::get();
  auto cpu_frequency = clock.get_frequency(hal::lpc40::peripheral::cpu);
  static hal::cortex_m::dwt_counter counter(cpu_frequency);

  // Setting Serial
  auto& uart0 = HAL_CHECK((hal::lpc40xx::uart::get<0, 64>(hal::serial::settings{
    .baud_rate = 38400,
  })));
  auto& uart1 = HAL_CHECK((hal::lpc40xx::uart::get<1, 8192>(hal::serial::settings{
    .baud_rate = 115200,
  })));

  // Setting I2C
  auto& i2c = HAL_CHECK((hal::lpc40xx::i2c::get<2>(hal::i2c::settings{
    .clock_rate = 100.0_kHz,
  })));

  // Setting CAN
  hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  auto& can = HAL_CHECK((hal::lpc40xx::can::get<2>(can_settings)));

  // Establishing Pins
  auto& in_pin0 = HAL_CHECK((hal::lpc40xx::input_pin::get<1, 15>()));
  auto& in_pin1 = HAL_CHECK((hal::lpc40xx::input_pin::get<1, 23>()));
  auto& in_pin2 = HAL_CHECK((hal::lpc40xx::input_pin::get<1, 22>()));

  auto& pwm_1_5 = HAL_CHECK((hal::lpc40xx::pwm::get<1, 5>()));

  auto& adc_4 = HAL_CHECK(hal::lpc40xx::adc::get<4>());
  auto& adc_5 = HAL_CHECK(hal::lpc40xx::adc::get<5>());

  // Setting Relay
  static const pwm_relay::settings relay_pwm_settings{ .frequency = 20.0_kHz, .initial_duty_cycle = 1.0f, .tapered_duty_cycle = 0.4f };
  static auto relay_pwm_pin = HAL_CHECK((hal::lpc40::pwm::get(2,5)));

  static auto power_saving_relay = HAL_CHECK(pwm_relay::create(relay_pwm_settings, relay_pwm_pin, counter));
  HAL_CHECK(power_saving_relay.toggle(true));

  return application_framework{ .terminal = &uart0,
                                .can = &can,
                                .in_pin0 = &in_pin0,
                                .in_pin1 = &in_pin1,
                                .in_pin2 = &in_pin2,
                                .pwm_1_6 = &pwm_1_6,
                                .pwm_1_5 = &pwm_1_5,
                                .adc_4 = &adc_4,
                                .adc_5 = &adc_5,
                                .esp = &uart1,
                                .i2c = &i2c,
                                .steady_clock = &counter,
                                .reset = []() { hal::cortex_m::reset(); },
                                .motor_relay = &power_saving_relay,
                              };
}

}
