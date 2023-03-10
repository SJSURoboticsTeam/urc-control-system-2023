#include <libhal-lpc40xx/pwm.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../hardware_map.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;

hal::status application(science::hardware_map& p_map)
{
  // science robot entry point here. This function may yield an error.
  // configure drivers
  auto& pwm = *p_map.pwm_1_6;
  auto& seal_pwm = *p_map.pwm_1_5;
  auto& steady_clock = *p_map.steady_clock;
  auto& i2c = *p_map.i2c;
  auto& terminal = *p_map.terminal;
  auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(i2c, 0b100'0000));
  HAL_CHECK(hal::delay(steady_clock, 10ms));
  auto pca_pwm_0 = pca9685.get_pwm_channel<0>();
  auto pca_pwm_1 = pca9685.get_pwm_channel<1>();
  auto pca_pwm_2 = pca9685.get_pwm_channel<2>();
  (void)hal::write(*p_map.terminal, "moving in 5\n");
  HAL_CHECK(pwm.frequency(50.0_Hz));
  HAL_CHECK(pca_pwm_0.frequency(1.50_kHz));
  HAL_CHECK(hal::delay(steady_clock, 10ms));
  HAL_CHECK(pca_pwm_1.frequency(1.50_kHz));
  HAL_CHECK(hal::delay(steady_clock, 10ms));
  HAL_CHECK(pca_pwm_2.frequency(1.50_kHz));
  HAL_CHECK(hal::delay(steady_clock, 1000ms));
  HAL_CHECK(pwm.duty_cycle(0.065f));
  HAL_CHECK(hal::delay(steady_clock, 1000ms));

  /*
  ================IMPORTANT===============
  The duty cycles on the pca devices assumes a 12V input.
  */

  // vacuum pump
  HAL_CHECK(hal::delay(steady_clock, 1000ms));
  HAL_CHECK(pca_pwm_0.duty_cycle(1.00f));

  // dosing pump 1
  HAL_CHECK(hal::delay(steady_clock, 1000ms));
  HAL_CHECK(pca_pwm_1.duty_cycle(0.50f));

  // dosing pump 2
  HAL_CHECK(hal::delay(steady_clock, 1000ms));
  HAL_CHECK(pca_pwm_2.duty_cycle(0.50f));

  /*
  =======END IMPORTANT NOTICE=========
  */

  while (true) {
    hal::write(terminal, "we are inside of da loop\n");

    /*
    ================IMPORTANT===============
    The duty cycles on the pca devices assumes a 12V input.
    */

    // vacuum pump
    // HAL_CHECK(pca_pwm_0.duty_cycle(1.0f));

    // servo
    //  HAL_CHECK(pwm.duty_cycle(0.065f));
    HAL_CHECK(hal::delay(steady_clock, 1000ms));

    // HAL_CHECK(pwm.duty_cycle(0.035f));
    // HAL_CHECK(hal::delay(steady_clock, 2000ms));

    // vacuum pump
    //  HAL_CHECK(pca_pwm_0.duty_cycle(1.00f));

    // dosing pump 1
    //  HAL_CHECK(pca_pwm_1.duty_cycle(0.50f));

    // dosing pump 2
    //  HAL_CHECK(pca_pwm_2.duty_cycle(0.50f));

    /*
    =======END IMPORTANT NOTICE=========
    */

    /*HAL_CHECK(pwm.duty_cycle(0.065f));
    HAL_CHECK(hal::delay(steady_clock, 10ms));
    HAL_CHECK(pca_pwm_0.duty_cycle(0.45f));
    HAL_CHECK(hal::delay(steady_clock, 10ms));
    HAL_CHECK(seal_pwm.duty_cycle(0.025f));
    HAL_CHECK(hal::delay(steady_clock, 5s));

    HAL_CHECK(pwm.duty_cycle(0.075f));
    HAL_CHECK(hal::delay(steady_clock, 10ms));
    HAL_CHECK(pca_pwm_0.duty_cycle(0.0f));
    HAL_CHECK(hal::delay(steady_clock, 10ms));
    HAL_CHECK(seal_pwm.duty_cycle(0.075f));
    HAL_CHECK(hal::delay(steady_clock, 5s));

    HAL_CHECK(pwm.duty_cycle(0.085f));
    HAL_CHECK(hal::delay(steady_clock, 10ms));
    HAL_CHECK(pca_pwm_0.duty_cycle(0.45f));
    HAL_CHECK(hal::delay(steady_clock, 10ms));
    HAL_CHECK(seal_pwm.duty_cycle(0.125f));
    HAL_CHECK(hal::delay(steady_clock, 5s));*/
  }

  return hal::success();
}