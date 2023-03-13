#include <libhal-mpu/mpu6050.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../hardware_map.hpp"

hal::status application(hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& clock = *p_map.clock;
  auto& console = *p_map.terminal;
  auto& i2c2 = *p_map.i2c2;
  auto& i2c1 = *p_map.i2c1;

  hal::print(console, "mpu Application Starting...\n\n");
  auto mpu1 = HAL_CHECK(hal::mpu::mpu6050::create(i2c2, 0x68));
  hal::print<128>(console, "Line 2 active\n");
  auto mpu2 = HAL_CHECK(hal::mpu::mpu6050::create(i2c1, 0x68));
  hal::print<128>(console, "Line 1 active\n");
  while (true) {
    (void)hal::delay(clock, 500ms);
    hal::print(console, "Reading acceleration... \n");
    HAL_CHECK(
      mpu1.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2));
    (void)hal::delay(clock, 500ms);
    auto acceleration = HAL_CHECK(mpu1.read());
    hal::print<128>(console,
                    "MPU 2 \t x = %fg, y = %fg, z = %fg \n",
                    acceleration.x,
                    acceleration.y,
                    acceleration.z);
    (void)hal::delay(clock, 500ms);

    hal::print(console, "Reading acceleration... \n");
    HAL_CHECK(
      mpu2.configure_full_scale(hal::mpu::mpu6050::max_acceleration::g2));
    (void)hal::delay(clock, 500ms);
    acceleration = HAL_CHECK(mpu2.read());
    hal::print<128>(console,
                    "MPU 1 \t x = %fg, y = %fg, z = %fg \n",
                    acceleration.x,
                    acceleration.y,
                    acceleration.z);
  }

  return hal::success();
}
