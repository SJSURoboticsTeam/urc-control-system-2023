#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40/can.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/input_pin.hpp>
#include <libhal-lpc40/uart.hpp>
#include <libhal-rmd/mc_x.hpp>
// #include <libhal-rmd/mc_x_servo.hpp>
#include <libhal-util/units.hpp>
// #include <libhal-pca/pca9685.hpp>
#include "../applications/application.hpp"
#include "../applications/arm_mission_control.cpp"
#include <libhal-lpc40/clock.hpp>
#include <libhal-lpc40/pwm.hpp>
#include <libhal-soft/rc_servo.hpp>

namespace sjsu::arm {

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

  // setting clock
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

  // servos, we need to init all of the mc_x motors then call make_servo
  // in order to pass servos into the application
  static hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  static auto can = HAL_CHECK((hal::lpc40::can::get(2, can_settings)));

  static auto can_router = hal::can_router::create(can).value();

  static auto rotunda_mc_x =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, counter, 36.0, 0x141));
  static auto rotunda_mc_x_servo =
    HAL_CHECK(hal::make_servo(rotunda_mc_x, 100.0_rpm));

  static auto shoulder_mc_x =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, counter, 36.0 * 65 / 30, 0x142));
  static auto shoulder_mc_x_servo =
    HAL_CHECK(hal::make_servo(shoulder_mc_x, 100.0_rpm));

  static auto elbow_mc_x =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, counter, 36.0 * 40 / 30, 0x143));
  static auto elbow_mc_x_servo =
    HAL_CHECK(hal::make_servo(elbow_mc_x, 100.0_rpm));

  static auto left_wrist_mc_x =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, counter, 36.0, 0x144));
  static auto left_wrist_mc_x_servo =
    HAL_CHECK(hal::make_servo(left_wrist_mc_x, 100.0_rpm));

  static auto right_wrist_mc_x =
    HAL_CHECK(hal::rmd::mc_x::create(can_router, counter, 36.0, 0x145));
  static auto right_wrist_mc_x_servo =
    HAL_CHECK(hal::make_servo(right_wrist_mc_x, 100.0_rpm));

  // static auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c, 0b100'0000));
  // static auto& pwm0 = pca9685.get_pwm_channel<0>();
  // auto servo_settings = hal::rc_servo::settings{
  //   .min_angle = 0.0_deg,
  //   .max_angle = 180.0_deg,
  //   .min_microseconds = 500,
  //   .max_microseconds = 2500,
  // };
  // static auto& end_effector_servo =
  //   HAL_CHECK(hal::rc_servo::create(pwm0, servo_settings))

    // mission control object
    static std::array<hal::byte, 8192>
      recieve_buffer1{};
  static auto uart1 = HAL_CHECK((hal::lpc40::uart::get(1,
                                                       recieve_buffer1,
                                                       hal::serial::settings{
                                                         .baud_rate = 115200,
                                                       })));

  static sjsu::arm::arm_mission_control arm_mc{};

  return application_framework{
    .rotunda_servo = &rotunda_mc_x_servo,
    .shoulder_servo = &shoulder_mc_x_servo,
    .elbow_servo = &elbow_mc_x_servo,
    .left_wrist_servo = &left_wrist_mc_x_servo,
    .right_wrist_servo = &right_wrist_mc_x_servo,
    // .end_effector = &end_effector_servo,
    .terminal = &uart0,
    .mc = &arm_mc,
    .clock = &counter,
    .reset = []() { hal::cortex_m::reset(); },
  };
}
}  // namespace sjsu::arm