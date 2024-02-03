//needs to have 2 methods control_strip, should turn on or off depending on command
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal-lpc40/output_pin.hpp>

#include <array>
#include "../applications/application.hpp"
#include "../include/sk9822.hpp"
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
hal::status light_change(effect_hardware hardware, hal::rgb_brightness color) {
    for(auto i = hardware.lights.begin(); i != hardware.lights.end(); i ++) {
      *i = color;
      hardware.driver->update(hardware.lights);
      hal::delay(*hardware.clock, 10ms);
    }
    return hal::success();
}

namespace sjsu::drive {
hal::status led_strip_controller(application_framework& p_resources, mission_control::mc_commands p_commands)
{
  using namespace std::literals;

  auto& clock = *p_resources.clock;
//   auto& console = *p_resources.terminal;

  auto clock_pin = HAL_CHECK(hal::lpc40::output_pin::get(1, 15)); //hope we have 2 more gpio pins here
  auto data_pin = HAL_CHECK(hal::lpc40::output_pin::get(1, 23)); //hope we have 2 more gpio pins here

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
//   HAL_CHECK(rampup_rampdown(hardware));
  if(p_commands.led_status == 1) { //turn on
    HAL_CHECK(light_change(hardware, hal::colors::RED));
  }
  else{
    HAL_CHECK(light_change(hardware, hal::colors::BLACK));

  }
//   return p_commands;
  return hal::success();
}
};