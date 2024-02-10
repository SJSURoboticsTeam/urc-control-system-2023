#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/pwm.hpp>
#include <libhal-lpc40/output_pin.hpp>

#include <array>
#include "../hardware_map.hpp"
#include "../../platform-implementations/sk9822.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;

struct effect_hardware {
  hal::light_strip_view lights;
  hal::sk9822 *driver;
  hal::steady_clock *clock;
};

/**
 * @brief WEE WOO WEE WOO NYPD!
 * 
 * it just flashes the light.
 * 
 * @param hardware 
 * @param on_value 
 * @param off_value 
 * @param period 
 * @return hal::status 
 */
hal::status beedoo_beedoo_beedoo(effect_hardware hardware, hal::rgb_brightness on_value, hal::rgb_brightness off_value, hal::time_duration period) {
  hal::time_duration half_period = period / 2;
  while(true) {
    hal::light_strip_util::set_all(hardware.lights, on_value);
    hardware.driver->update(hardware.lights);
    hal::delay(*(hardware.clock), half_period);
    hal::light_strip_util::set_all(hardware.lights, off_value);
    hardware.driver->update(hardware.lights);
    hal::delay(*(hardware.clock), half_period);
  }
}

/**
 * @brief Each led should each turn on one after each other and then turn off one after each other
 * 
 * @param hardware 
 * @return hal::status 
 */
hal::status rampup_rampdown(effect_hardware hardware) {
  while (true) {
    for(auto i = hardware.lights.begin(); i != hardware.lights.end(); i ++) {
      *i = hal::colors::WHITE;
      hardware.driver->update(hardware.lights);
      hal::delay(*hardware.clock, 10ms);
    }
    for(auto i = hardware.lights.rbegin(); i != hardware.lights.rend(); i ++) {
      *i = hal::colors::BLACK;
      hardware.driver->update(hardware.lights);
      hal::delay(*hardware.clock, 10ms);
    }
  }
}

namespace sjsu::science {
hal::status application(application_framework& p_resources)
{
  using namespace std::literals;

  auto& clock = *p_resources.steady_clock;
//   auto& console = *p_resources.terminal;

  auto clock_pin = HAL_CHECK(hal::lpc40::output_pin::get(1, 15));
  auto data_pin = HAL_CHECK(hal::lpc40::output_pin::get(1, 23));

  hal::light_strip<35> lights;
  hal::sk9822 driver(clock_pin, data_pin, clock);
  hal::light_strip_util::set_all(lights, hal::colors::BLACK);

  hal::rgb_brightness ON, OFF;
  ON.set(0xff, 0x00, 0x00, 0b11111);
  OFF.set(0, 0, 0, 0);

  effect_hardware hardware;
  hardware.clock = &clock;
  hardware.lights = lights;
  hardware.driver = &driver;

  // HAL_CHECK(beedoo_beedoo_beedoo(hardware, hal::color::red, hal::color::black, 100ms));
  HAL_CHECK(rampup_rampdown(hardware));

  while (true) {
    // Print message
    // hal::print(console, "Hello, World\n");

    // for(int i = 0; i  < 35; i ++ ) {
    //   lights[i].set(0xff, 0xff, 0xff, 0b111);
    //   lights.update(); 
    //   hal::delay(clock, 10ms);
    //   hal::print(console, ".");
    // }
    // for(int i = 34; i >= 0; i -- ) {
    //   lights[i].set(0x00,0x00,0x00, 0b000);
    //   lights.update(); 
    //   hal::delay(clock, 10ms);
    //   hal::print(console, ".");
    // }
    // hal::print(console, "\n");
  
  }
}
};