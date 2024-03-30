#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>

#include <libhal-lpc40/can.hpp>
#include <libhal-lpc40/constants.hpp>
#include <libhal-lpc40/i2c.hpp>
#include <libhal-lpc40/input_pin.hpp>
#include <libhal-lpc40/uart.hpp>

#include <libhal-mpu/mpu6050.hpp>
#include <libhal-rmd/mc_x.hpp>

#include <libhal-util/units.hpp>
// #include <libhal-pca/pca9685.hpp>
#include "../applications/application.hpp"
#include "../platform-implementations/esp8266_mission_control.cpp"
#include "../platform-implementations/helper.hpp"
#include "../platform-implementations/home.hpp"
#include "../platform-implementations/offset_servo.hpp"

#include <libhal-lpc40/clock.hpp>
#include <libhal-lpc40/pwm.hpp>
#include <libhal-soft/inert_drivers/inert_accelerometer.hpp>
#include <libhal-soft/rc_servo.hpp>
#include <libhal/error.hpp>
#include <libhal/timeout.hpp>

namespace sjsu::arm {

void initialize_processor()
{

  hal::cortex_m::initialize_data_section();
  hal::cortex_m::initialize_floating_point_unit();
}

application_framework initialize_platform()
{
  using namespace hal::literals;
  using namespace std::chrono_literals;

  // setting clock
  hal::lpc40::maximum(12.0_MHz);

  hal::lpc40::i2c i2c1(1,
                       hal::i2c::settings{
                         .clock_rate = 100.0_kHz,
                       });
  hal::lpc40::i2c i2c2(2,
                      hal::i2c::settings{
                        .clock_rate = 100.0_kHz,
                      });

  auto cpu_frequency = hal::lpc40::get_frequency(hal::lpc40::peripheral::cpu);
  static hal::cortex_m::dwt_counter counter(cpu_frequency);

  // Serial
  static std::array<hal::byte, 1024> recieve_buffer0{};
  hal::lpc40::uart uart0(0,
                         recieve_buffer0,
                         hal::serial::settings{
                           .baud_rate = 38400,
                         });

  // servos, we need to init all of the mc_x motors then call make_servo
  // in order to pass servos into the application
  static hal::can::settings can_settings{ .baud_rate = 1.0_MHz };
  hal::lpc40::can can(2, can_settings);

  hal::can_router can_router(can);

  hal::rmd::mc_x rotunda_mc_x(can_router, counter, 36.0, 0x141);
  static auto rotunda_servo = hal::make_servo(rotunda_mc_x, 2.0_rpm);

  hal::rmd::mc_x shoulder_mc_x(can_router, counter, 36.0 * 65 / 30, 0x142);
  static auto shoulder_servo = hal::make_servo(shoulder_mc_x, 5.0_rpm);

  hal::rmd::mc_x elbow_mc_x(can_router, counter, 36.0 * 40 / 30, 0x143);
  static auto elbow_servo = hal::make_servo(elbow_mc_x, 5.0_rpm);

  hal::rmd::mc_x left_wrist_mc_x(can_router, counter, 36.0, 0x144);
  static auto left_wrist_servo = hal::make_servo(left_wrist_mc_x, 2.0_rpm);

  hal::rmd::mc_x right_wrist_mc_x(can_router, counter, 36.0, 0x145);
  static auto right_wrist_servo = hal::make_servo(right_wrist_mc_x, 2.0_rpm);

  // static auto pca9685 = hal::pca::pca9685::create(i2c,
  // 0b100'0000)); static auto& pwm0 = pca9685.get_pwm_channel<0>(); auto
  // servo_settings = hal::rc_servo::settings{
  //   .min_angle = 0.0_deg,
  //   .max_angle = 180.0_deg,
  //   .min_microseconds = 500,
  //   .max_microseconds = 2500,
  // };
  // static auto& end_effector_servo =
  //   hal::rc_servo::create(pwm0, servo_settings))

  // mission control object
  // mission control object

  // MPU INIT
  static std::array<hal::byte, 8192> recieve_buffer1{};

  hal::lpc40::uart uart1(1,
                         recieve_buffer1,
                         hal::serial::settings{
                           .baud_rate = 115200,
                         });

  static constexpr std::string_view ssid =
    "TP-Link_FC30";  // change to wifi name that you are using
  static constexpr std::string_view password =
    "R0Bot1cs3250";  // change to wifi password you are using

  // still need to decide what we want the static IP to be
  static constexpr std::string_view ip = "192.168.0.212";
  static constexpr auto socket_config = hal::esp8266::at::socket_config{
    .type = hal::esp8266::at::socket_type::tcp,
    .domain = "192.168.0.211",
    .port = 5000,
  };
  hal::write(uart0, "created Socket\n", hal::never_timeout());
  static constexpr std::string_view get_request = "GET /arm HTTP/1.1\r\n"
                                                  "Host: 192.168.0.211:5000\r\n"
                                                  "Keep-Alive: timeout=1000\r\n"
                                                  "Connection: keep-alive\r\n"
                                                  "\r\n";

  static std::array<hal::byte, 2048> buffer{};
  static auto helper = serial_mirror(uart1, uart0);

  auto timeout = hal::create_timeout(counter, 10s);
  hal::esp8266::at esp8266(helper, timeout);
  sjsu::arm::esp8266_mission_control* arm_mission_control = nullptr;
  while (true) {
    try {
      auto mc_timeout = hal::create_timeout(counter, 10s);
      static sjsu::arm::esp8266_mission_control esp_mission_control(
        esp8266,
        uart0,
        ssid,
        password,
        socket_config,
        ip,
        buffer,
        get_request,
        mc_timeout);

      arm_mission_control = &esp_mission_control;
      break;
    } catch (const hal::timed_out&) {
      continue;
    }
  }

  hal::mpu::mpu6050 elbow_mpu(i2c1, hal::mpu::mpu6050::address_ground);
  elbow_mpu.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2);
  hal::print(uart0, "Elbow INIT\n");

  hal::mpu::mpu6050 shoulder_mpu(i2c1, hal::mpu::mpu6050::address_voltage_high);
  shoulder_mpu.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2);
  hal::print(uart0, "Shoulder INIT\n");

  hal::mpu::mpu6050 rotunda_mpu(i2c2, hal::mpu::mpu6050::address_voltage_high);
  rotunda_mpu.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2);
  hal::print(uart0, "Rotunda INIT\n");

  auto zero_a = rotunda_mpu.read();
  hal::soft::inert_accelerometer wrist_mpu(zero_a);

  home(rotunda_mpu,
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
       counter);

  hal::delay(counter, 1s);

  return application_framework{
    .rotunda_servo = &rotunda_servo,
    .shoulder_servo = &shoulder_servo,
    .elbow_servo = &elbow_servo,
    .left_wrist_servo = &left_wrist_servo,
    .right_wrist_servo = &right_wrist_servo,

    .rotunda_accelerometer = &rotunda_mpu,
    .shoulder_accelerometer = &shoulder_mpu,
    .elbow_accelerometer = &elbow_mpu,
    .wrist_accelerometer = &wrist_mpu,

    // .end_effector = &end_effector_servo,
    .terminal = &uart0,
    .mc = arm_mission_control,
    .clock = &counter,
    .reset = []() { hal::cortex_m::reset(); },
  };
}
}  // namespace sjsu::arm