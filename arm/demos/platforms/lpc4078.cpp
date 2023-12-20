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

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40/can.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/i2c.hpp>
#include <libhal-lpc40/uart.hpp>

#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/can.hpp>

#include <libhal-mpu/mpu6050.hpp>
#include <libhal-soft/inert_drivers/inert_accelerometer.hpp>

#include "../../platform-implementations/home.hpp"
#include "../../platform-implementations/offset_servo.hpp"
#include <libhal-rmd/mc_x.hpp>
#include <algorithm>

#include "../applications/application.hpp"
#include "../helpers/print_offsets.hpp"

namespace sjsu::arm {
hal::status initialize_processor()
{
  hal::cortex_m::initialize_data_section();
  hal::cortex_m::initialize_floating_point_unit();

  return hal::success();
}

hal::result<sjsu::arm::application_framework> initialize_platform()
{
  using namespace std::chrono_literals;
  using namespace hal::literals;
  // Set the MCU to the maximum clock speed
  HAL_CHECK(hal::lpc40::clock::maximum(12.0_MHz));

  auto& clock = hal::lpc40::clock::get();
  auto cpu_frequency = clock.get_frequency(hal::lpc40::peripheral::cpu);
  static hal::cortex_m::dwt_counter counter(cpu_frequency);

  static std::array<hal::byte, 64> receive_buffer{};
  static auto uart0 = HAL_CHECK((hal::lpc40::uart::get(0,
                                                       receive_buffer,
                                                       hal::serial::settings{
                                                         .baud_rate = 38400,
                                                       })));

  static auto i2c1 = HAL_CHECK((hal::lpc40::i2c::get(1,
                                                     hal::i2c::settings{
                                                       .clock_rate = 100.0_kHz,
                                                     })));

  static auto i2c2 = HAL_CHECK((hal::lpc40::i2c::get(2,
                                                     hal::i2c::settings{
                                                       .clock_rate = 100.0_kHz,
                                                     })));

  hal::print(uart0, "MPU INIT\n");
  static auto elbow_mpu = HAL_CHECK(
    hal::mpu::mpu6050::create(i2c1, hal::mpu::mpu6050::address_ground));
  HAL_CHECK(
    elbow_mpu.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2));
  hal::print(uart0, "Elbow INIT\n");

  static auto shoulder_mpu = HAL_CHECK(
    hal::mpu::mpu6050::create(i2c1, hal::mpu::mpu6050::address_voltage_high));
  HAL_CHECK(
    shoulder_mpu.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2));
  hal::print(uart0, "Shoulder INIT\n");

  static auto rotunda_mpu = HAL_CHECK(
    hal::mpu::mpu6050::create(i2c2, hal::mpu::mpu6050::address_voltage_high));
  HAL_CHECK(
    rotunda_mpu.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2));
  hal::print(uart0, "Rot INIT\n");

  auto zero_a = HAL_CHECK(rotunda_mpu.read());
  static auto wrist_mpu =
    HAL_CHECK(hal::soft::inert_accelerometer::create(zero_a));

  static hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  static auto can = HAL_CHECK((hal::lpc40::can::get(2, can_settings)));

  static auto can_router = hal::can_router::create(can).value();

  static auto rotunda_mc_x =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, counter, 36.0, 0x141));
  static auto rotunda_mc_x_servo =
    HAL_CHECK(hal::make_servo(rotunda_mc_x, 2.0_rpm));

  static auto shoulder_mc_x = HAL_CHECK(
    hal::rmd::mc_x::create(can_router, counter, 36.0 * 65 / 30, 0x142));
  static auto shoulder_mc_x_servo =
    HAL_CHECK(hal::make_servo(shoulder_mc_x, 2.0_rpm));

  static auto elbow_mc_x = HAL_CHECK(
    hal::rmd::mc_x::create(can_router, counter, 36.0 * 40 / 30, 0x143));
  static auto elbow_mc_x_servo =
    HAL_CHECK(hal::make_servo(elbow_mc_x, 2.0_rpm));

  static auto left_wrist_mc_x =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, counter, 36.0, 0x144));
  static auto left_wrist_mc_x_servo =
    HAL_CHECK(hal::make_servo(left_wrist_mc_x, 2.0_rpm));

  static auto right_wrist_mc_x =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, counter, 36.0, 0x145));
  static auto right_wrist_mc_x_servo =
    HAL_CHECK(hal::make_servo(right_wrist_mc_x, 2.0_rpm));

  auto rotunda_offset = rotunda_mc_x.feedback().encoder;
  hal::delay(counter, 10ms);
  auto shoulder_offset = shoulder_mc_x.feedback().encoder;
  hal::delay(counter, 10ms);
  auto elbow_offset = elbow_mc_x.feedback().encoder;
  hal::delay(counter, 10ms);
  auto left_wrist_offset = left_wrist_mc_x.feedback().encoder;
  hal::delay(counter, 10ms);
  auto right_wrist_offset = right_wrist_mc_x.feedback().encoder;
  hal::delay(counter, 10ms);

  static auto rotunda_offset_servo =
    HAL_CHECK(offset_servo::create(rotunda_mc_x_servo, 0));
  static auto elbow_offset_servo =
    HAL_CHECK(offset_servo::create(elbow_mc_x_servo, 0));
  static auto shoulder_offset_servo =
    HAL_CHECK(offset_servo::create(shoulder_mc_x_servo, 0));
  static auto left_wrist_offset_servo =
    HAL_CHECK(offset_servo::create(left_wrist_mc_x_servo, 0));
  static auto right_wrist_offset_servo =
    HAL_CHECK(offset_servo::create(right_wrist_mc_x_servo, 0));

  HAL_CHECK(home(rotunda_mpu,
                 shoulder_mpu,
                 elbow_mpu,
                 wrist_mpu,
                 rotunda_mc_x,
                 shoulder_mc_x,
                 elbow_mc_x,
                 left_wrist_mc_x,
                 right_wrist_mc_x,
                 uart0,
                 can,
                 counter));

  hal::delay(counter, 1s);  

  return sjsu::arm::application_framework{

    .rotunda_accelerometer = &rotunda_mpu,
    .shoulder_accelerometer = &shoulder_mpu,
    .elbow_accelerometer = &elbow_mpu,
    .wrist_accelerometer = &wrist_mpu,

    .terminal = &uart0,
    .clock = &counter,
    .reset = []() { hal::cortex_m::reset(); },
  };
}
}  // namespace sjsu::arm