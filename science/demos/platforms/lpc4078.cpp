// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string_view>

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40/can.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/input_pin.hpp>
#include <libhal-lpc40/uart.hpp>
#include <libhal-lpc40/i2c.hpp>

#include <libhal-rmd/mc_x.hpp>
#include <libhal-util/units.hpp>
#include <libhal-pca/pca9685.hpp>

#include "../hardware_map.hpp"

#include <libhal-lpc40/pwm.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-soft/rc_servo.hpp>

#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../hardware_map.hpp"

#include <string_view>
#include <array>
#include <cinttypes>
namespace sjsu::science 
{
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

    // Set the MCU to the maximum clock speed
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

    static auto i2c = HAL_CHECK(hal::lpc40::i2c::get(2));

    static auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c,0b100'0000)); 
    static auto pwm0 = pca9685.get_pwm_channel<3>(); 
    static auto rotunda_science_settings = hal::soft::rc_servo::settings
    {
      .min_angle = hal::degrees(0.0),
      .max_angle = hal::degrees(180.0),
      .min_microseconds = 1000,
      .max_microseconds = 2000,
    };

    static auto rotunda_science_servo =
      HAL_CHECK(hal::soft::rc_servo::create(pwm0, rotunda_science_settings));

    return application_framework
    { .rotunda_science = &rotunda_science_servo,
      .terminal = &uart0,
      .clock = &counter,
      .reset = []() { hal::cortex_m::reset(); },
    };
  }
}  // namespace sjsu::science
