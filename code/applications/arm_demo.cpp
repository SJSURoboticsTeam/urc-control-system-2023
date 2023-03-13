#include <libhal-lpc40xx/can.hpp>
#include <libhal-mpu/mpu6050.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal-rmd/drc.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../hardware_map.hpp"

float convert_to_duty_cycle(int angle)
{
  std::pair<float, float> from;
  std::pair<float, float> to;
  from.first = 0.0f;
  from.second = 180.0f;
  to.first = 0.065f;
  to.second = 0.085f;
  return hal::map(static_cast<float>(angle), from, to);
}

hal::status application(sjsu::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& clock = *p_map.steady_clock;
  auto& console = *p_map.terminal;
  auto& i2c1 = *p_map.i2c1;
  auto& i2c2 = *p_map.i2c2;
  auto& can = *p_map.can;
  hal::accelerometer::read_t acceleration;

  hal::print(console, "mpu Application Starting...\n\n");
  auto mpu1 = HAL_CHECK(hal::mpu::mpu6050::create(i2c1, 0x68));
  (void)hal::delay(clock, 500ms);
  hal::print<128>(console, "Line 1 active\n");
  auto mpu2 = HAL_CHECK(hal::mpu::mpu6050::create(i2c2, 0x68));
  (void)hal::delay(clock, 500ms);
  // auto mpu3 = HAL_CHECK(hal::mpu::mpu6050::create(i2c2, 0x69));
  (void)hal::delay(clock, 500ms);
  hal::print<128>(console, "Line 2 active\n");
  // auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c1, 0b100'0000));
  // auto pwm0 = pca9685.get_pwm_channel<0>();
  // HAL_CHECK(pwm0.frequency(50.0_Hz));
  hal::print(console, "pca9685 active\n\n");
  HAL_CHECK(hal::delay(clock, 500ms));
  HAL_CHECK(mpu2.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2));
  HAL_CHECK(mpu1.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2));
  // HAL_CHECK(mpu3.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2));
  HAL_CHECK(hal::delay(clock, 500ms));
  // auto can_router = hal::can_router::create(can).value();
  // auto motor = HAL_CHECK(hal::rmd::drc::create(can_router, clock, 8.0,
  // 0x141));
  HAL_CHECK(hal::delay(clock, 500ms));
  while (true) {
    HAL_CHECK(hal::delay(clock, 50ms));
    // setting rmd speed
    // motor.velocity_control(10.0_rpm);
    HAL_CHECK(hal::delay(clock, 10ms));
    // setting pwm on pca
    // HAL_CHECK(pwm0.duty_cycle(convert_to_duty_cycle(0)));
    HAL_CHECK(hal::delay(clock, 500ms));
    // HAL_CHECK(pwm0.duty_cycle(convert_to_duty_cycle(180)));
    HAL_CHECK(hal::delay(clock, 10ms));

    // reading data from mpus
    hal::print(console, "Reading acceleration... \n");
    acceleration = HAL_CHECK(mpu1.read());
    hal::print<128>(console,
                    "MPU 0 \t x = %fg, y = %fg, z = %fg \n",
                    acceleration.x,
                    acceleration.y,
                    acceleration.z);
    (void)hal::delay(clock, 500ms);

    hal::print(console, "Reading acceleration... \n");
    (void)hal::delay(clock, 500ms);
    // acceleration = HAL_CHECK(mpu3.read());
    hal::print<128>(console,
                    "MPU 0 \t x = %fg, y = %fg, z = %fg \n",
                    acceleration.x,
                    acceleration.y,
                    acceleration.z);
    (void)hal::delay(clock, 500ms);

    hal::print(console, "Reading acceleration... \n");
    acceleration = HAL_CHECK(mpu2.read());
    hal::print<128>(console,
                    "MPU 1 \t x = %fg, y = %fg, z = %fg \n",
                    acceleration.x,
                    acceleration.y,
                    acceleration.z);
  }

  return hal::success();
}
