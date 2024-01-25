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
#include <libhal-util/serial.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/can.hpp>


#include <libhal-rmd/mc_x.hpp>
#include "../applications/application.hpp"


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

  // static auto i2c1 = HAL_CHECK((hal::lpc40::i2c::get(1,
  //                                                    hal::i2c::settings{
  //                                                      .clock_rate = 100.0_kHz,
  //                                                    })));

  // static auto i2c2 = HAL_CHECK((hal::lpc40::i2c::get(2,
  //                                                    hal::i2c::settings{
  //                                                      .clock_rate = 100.0_kHz,
  //                                                    })));

  // hal::print(uart0, "MPU INIT\n");
  // static auto elbow_mpu = HAL_CHECK(
  //   hal::mpu::mpu6050::create(i2c1, hal::mpu::mpu6050::address_ground));
  // HAL_CHECK(
  //   elbow_mpu.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2));
  // hal::print(uart0, "Elbow INIT\n");

  // static auto shoulder_mpu = HAL_CHECK(
  //   hal::mpu::mpu6050::create(i2c1, hal::mpu::mpu6050::address_voltage_high));
  // HAL_CHECK(
  //   shoulder_mpu.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2));
  // hal::print(uart0, "Shoulder INIT\n");

  // static auto rotunda_mpu = HAL_CHECK(
  //   hal::mpu::mpu6050::create(i2c2, hal::mpu::mpu6050::address_voltage_high));
  // HAL_CHECK(
  //   rotunda_mpu.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2));
  // hal::print(uart0, "Rot INIT\n");

  // auto zero_a = HAL_CHECK(rotunda_mpu.read());
  // static auto wrist_mpu =
  //   HAL_CHECK(hal::soft::inert_accelerometer::create(zero_a));

  static hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  static auto can = HAL_CHECK((hal::lpc40::can::get(2, can_settings)));

  static auto can_router = hal::can_router::create(can).value();

  // static auto rotunda_mc_x =
  //   HAL_CHECK(hal::rmd::mc_x::create(can_router, counter, 36.0, 0x141));
  // static auto rotunda_mc_x_servo =
  //   HAL_CHECK(hal::make_servo(rotunda_mc_x, 2.0_rpm));

  // static auto shoulder_mc_x = HAL_CHECK(
  //   hal::rmd::mc_x::create(can_router, counter, 36.0 * 65 / 30, 0x142));
  // static auto shoulder_mc_x_servo =
  //   HAL_CHECK(hal::make_servo(shoulder_mc_x, 2.0_rpm));

  // static auto elbow_mc_x = HAL_CHECK(
  //   hal::rmd::mc_x::create(can_router, counter, 36.0 * 40 / 30, 0x143));
  // static auto elbow_mc_x_servo =
  //   HAL_CHECK(hal::make_servo(elbow_mc_x, 2.0_rpm));

  // static auto left_wrist_mc_x =
  //   HAL_CHECK(hal::rmd::mc_x::create(can_router, counter, 36.0, 0x144));
  // static auto left_wrist_mc_x_servo =
  //   HAL_CHECK(hal::make_servo(left_wrist_mc_x, 2.0_rpm));

  // static auto right_wrist_mc_x =
  //   HAL_CHECK(hal::rmd::mc_x::create(can_router, counter, 36.0, 0x145));
  // static auto right_wrist_mc_x_servo =
  //   HAL_CHECK(hal::make_servo(right_wrist_mc_x, 2.0_rpm));

  // hal::print(uart0, "REQUESTING FB\n");
  // HAL_CHECK(
  //   rotunda_mc_x.feedback_request(hal::rmd::mc_x::read::multi_turns_angle));
  // hal::print(uart0, "ROTUNDA SUCCESS\n");
  // hal::delay(counter, 10ms);
  // HAL_CHECK(
  //   shoulder_mc_x.feedback_request(hal::rmd::mc_x::read::multi_turns_angle));
  // hal::print(uart0, "SHOULDER SUCCESS\n");
  // hal::delay(counter, 10ms);
  // HAL_CHECK(
  //   elbow_mc_x.feedback_request(hal::rmd::mc_x::read::multi_turns_angle));
  // hal::print(uart0, "WLBOW SUCCESS\n");
  // hal::delay(counter, 10ms);
  // HAL_CHECK(
  //   left_wrist_mc_x.feedback_request(hal::rmd::mc_x::read::multi_turns_angle));
  // hal::print(uart0, "LWRIST SUCCESS\n");
  // hal::delay(counter, 10ms);
  // HAL_CHECK(
  //   right_wrist_mc_x.feedback_request(hal::rmd::mc_x::read::multi_turns_angle));
  // hal::print(uart0, "RWRIST SUCCESS\n");
  // hal::delay(counter, 10ms);

  // auto rotunda_offset = rotunda_mc_x.feedback().encoder;
  // hal::delay(counter, 10ms);
  // auto shoulder_offset = shoulder_mc_x.feedback().encoder;
  // hal::delay(counter, 10ms);
  // auto elbow_offset = elbow_mc_x.feedback().encoder;
  // hal::delay(counter, 10ms);
  // auto left_wrist_offset = left_wrist_mc_x.feedback().encoder;
  // hal::delay(counter, 10ms);
  // auto right_wrist_offset = right_wrist_mc_x.feedback().encoder;
  // hal::delay(counter, 10ms);

  // hal::print<1024>(uart0,
  //                  "encoders\nR: %d\nS: %d\nE: %d\nLWR: %d\nRWR: %d\n",
  //                  rotunda_offset,
  //                  shoulder_offset,
  //                  elbow_offset,
  //                  left_wrist_offset,
  //                  right_wrist_offset);

  // static auto rotunda_offset_servo =
  //   HAL_CHECK(offset_servo::create(rotunda_mc_x_servo, 0));
  // static auto elbow_offset_servo =
  //   HAL_CHECK(offset_servo::create(elbow_mc_x_servo, 0));
  // static auto shoulder_offset_servo =
  //   HAL_CHECK(offset_servo::create(shoulder_mc_x_servo, 0));
  // static auto left_wrist_offset_servo =
  //   HAL_CHECK(offset_servo::create(left_wrist_mc_x_servo, 0));
  // static auto right_wrist_offset_servo =
  //   HAL_CHECK(offset_servo::create(right_wrist_mc_x_servo, 0));

  // hal::print(uart0, "\nBEFORE: \n");
  // print_arm(rotunda_offset_servo,
  //           shoulder_offset_servo,
  //           elbow_offset_servo,
  //           left_wrist_offset_servo,
  //           right_wrist_offset_servo,
  //           uart0);

  // hal::can::message_t set_zero;
  // set_zero.id = 0x142;
  // set_zero.payload = {0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  // set_zero.length = 8;
  // HAL_CHECK(can.send(set_zero));
  // uart0.read

  // hal::delay(counter, 5s);
  // HAL_CHECK(shoulder_offset_servo.position(-45));

  // HAL_CHECK(home(rotunda_mpu,
  //                shoulder_mpu,
  //                elbow_mpu,
  //                wrist_mpu,
  //                rotunda_offset_servo,
  //                shoulder_offset_servo,
  //                elbow_offset_servo,
  //                left_wrist_offset_servo,
  //                right_wrist_offset_servo,
  //                uart0,
  //                counter));

  return sjsu::arm::application_framework{
    .router = &can_router,

    .terminal = &uart0,
    .clock = &counter,
    .reset = []() { hal::cortex_m::reset(); },
  };
}
}  // namespace sjsu::arm