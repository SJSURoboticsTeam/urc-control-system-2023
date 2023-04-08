#include <libhal-lpc40xx/pwm.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal-util/map.hpp>

#include "../../hardware_map.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;
float ConvertAngleToDutyCycle(int angle)
{
  std::pair<float, float> from;
  std::pair<float, float> to;
  from.first = 0.0f;
  from.second = 180.0f;
  to.first = 0.6f;
  to.second = 0.9f;
  return hal::map(static_cast<float>(angle), from, to);
}
hal::status application(sjsu::hardware_map& p_map)
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
  HAL_CHECK(seal_pwm.frequency(666.0_Hz));
  HAL_CHECK(pca_pwm_0.frequency(1.50_kHz));
  HAL_CHECK(hal::delay(steady_clock, 10ms));
  HAL_CHECK(pca_pwm_1.frequency(1.50_kHz));
  HAL_CHECK(hal::delay(steady_clock, 10ms));
  HAL_CHECK(pca_pwm_2.frequency(1.50_kHz));
  HAL_CHECK(hal::delay(steady_clock, 1000ms));
  HAL_CHECK(pwm.duty_cycle(0.065f));
  HAL_CHECK(hal::delay(steady_clock, 1000ms));
//20 ms ->1.5ms (1500 micro)/20ms
//330 Hz ->3.03 ms (0.5/3.03) 0.165
//3.03ms -> ()2.5/3.03 ->0.825
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

//  HAL_CHECK(seal_pwm.duty_cycle(ConvertAngleToDutyCycle(0)));

  while (true) {
    hal::write(terminal, "we are inside of da loop\n");
    // HAL_CHECK(hal::delay(steady_clock, 10ms));
    // HAL_CHECK(seal_pwm.duty_cycle(ConvertAngleToDutyCycle(0)));
    // hal::write(terminal, "max sealing (presumably going right)\n");

    // HAL_CHECK(hal::delay(steady_clock, 1000ms));

    HAL_CHECK(seal_pwm.duty_cycle(ConvertAngleToDutyCycle(180))); //changing to another servo test
    // hal::write(terminal, "min sealing (presumably going left)\n");

    HAL_CHECK(hal::delay(steady_clock, 1000ms));
     hal::write(terminal, "turn the other side angle\n");

    HAL_CHECK(seal_pwm.duty_cycle(ConvertAngleToDutyCycle(0))); //changing to another servo test

    HAL_CHECK(hal::delay(steady_clock, 1000ms));



    /*
    ================IMPORTANT===============
    The duty cycles on the pca devices assumes a 12V input.
    */

    // vacuum pump
    // HAL_CHECK(pca_pwm_0.duty_cycle(1.0f));

    // servo
    //  HAL_CHECK(pwm.duty_cycle(0.065f));
    

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