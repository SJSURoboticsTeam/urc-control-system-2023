#include "../include/sk9822.hpp"
namespace sjsu::drive {

using namespace hal::literals;
using namespace std::chrono_literals;

// const rgb_brightness RED = rgb_brightness(0xff, 0x00, 0x00, 0b11111);
// const rgb_brightness GREEN = rgb_brightness(0x00, 0xff, 0x00, 0b11111);
// const rgb_brightness BLUE = rgb_brightness(0x00, 0x00, 0xff, 0b11111);
// const rgb_brightness WHITE = rgb_brightness(0xff, 0xff, 0xff, 0b11111);
// const rgb_brightness BLACK = rgb_brightness(0x00, 0x00, 0x00, 0x00);

void set_all(sjsu::drive::light_strip_view lights,
             const hal::byte r,
             const hal::byte g,
             const hal::byte b,
             const hal::byte brightness)
{

  rgb_brightness setting;
  setting.r = r;
  setting.g = g;
  setting.b = b;
  setting.brightness = brightness;
  for (auto i = lights.begin(); i != lights.end(); i++) {
    *i = setting;
  }
}

void set_all(sjsu::drive::light_strip_view lights, const rgb_brightness value)
{
  for (auto i = lights.begin(); i != lights.end(); i++) {
    *i = value;
  }
}

sk9822::sk9822(hal::output_pin& p_clock_pin,
               hal::output_pin& p_data_pin,
               hal::steady_clock& p_clock)
{
  clock_pin = &p_clock_pin;
  data_pin = &p_data_pin;
  clock = &p_clock;
}

hal::status sk9822::update(sjsu::drive::light_strip_view lights)
{
  HAL_CHECK(send_byte(0x00));
  HAL_CHECK(send_byte(0x00));
  HAL_CHECK(send_byte(0x00));
  HAL_CHECK(send_byte(0x00));

  for (auto i = lights.begin(); i != lights.end(); i++) {
    HAL_CHECK(send_byte((*i).brightness | 0b11100000));
    HAL_CHECK(send_byte((*i).b));
    HAL_CHECK(send_byte((*i).g));
    HAL_CHECK(send_byte((*i).r));
  }

  HAL_CHECK(send_byte(0xff));
  HAL_CHECK(send_byte(0xff));
  HAL_CHECK(send_byte(0xff));
  HAL_CHECK(send_byte(0xff));

  return hal::success();
}

hal::status sk9822::update(sjsu::drive::light_strip_view lights,
                           hal::byte overriding_brightness)
{
  HAL_CHECK(send_byte(0x00));
  HAL_CHECK(send_byte(0x00));
  HAL_CHECK(send_byte(0x00));
  HAL_CHECK(send_byte(0x00));

  for (auto i = lights.begin(); i != lights.end(); i++) {
    HAL_CHECK(send_byte(overriding_brightness | 0b11100000));
    HAL_CHECK(send_byte((*i).b));
    HAL_CHECK(send_byte((*i).g));
    HAL_CHECK(send_byte((*i).r));
  }

  HAL_CHECK(send_byte(0xff));
  HAL_CHECK(send_byte(0xff));
  HAL_CHECK(send_byte(0xff));
  HAL_CHECK(send_byte(0xff));

  return hal::success();
}

hal::status sk9822::send_byte(hal::byte data)
{
  for (int i = 0; i < 8; i++) {
    if (data & (1 << i)) {
      HAL_CHECK((*data_pin).level(true));
    } else {
      HAL_CHECK((*data_pin).level(false));
    }
    hal::delay(*clock, half_period);
    HAL_CHECK((*clock_pin).level(true));
    hal::delay(*clock, period);
    HAL_CHECK((*clock_pin).level(false));
    hal::delay(*clock, half_period);
  }

  return hal::success();
}
// hal::status led_strip_controller(application_framework& p_resources,
//                                  mission_control::mc_commands p_commands)
// {
//   using namespace std::literals;

//   auto& clock = *p_resources.clock;
//   //   auto& console = *p_resources.terminal;

//   auto clock_pin = HAL_CHECK(
//     hal::lpc40::output_pin::get(1, 22));  // hope we have 2 more gpio pins
//     here
//   auto data_pin = HAL_CHECK(
//     hal::lpc40::output_pin::get(1, 20));  // hope we have 2 more gpio pins
//     here

//   hal::light_strip<35> lights;
//   hal::sk9822 driver(clock_pin, data_pin, clock);
//   hal::light_strip_util::set_all(lights, BLACK);

//   hal::rgb_brightness ON, OFF;
//   ON.set(0xff, 0x00, 0x00, 0b11111);
//   OFF.set(0, 0, 0, 0);

//   effect_hardware hardware;
//   hardware.clock = &clock;
//   hardware.lights = lights;
//   hardware.driver = &driver;

//   if (p_commands.led_status == 1) {  // red - autonomous driving
//     HAL_CHECK(light_change(hardware, RED));
//   } else if (p_commands.led_status == 2) {  // blue - manually driving
//     HAL_CHECK(light_change(hardware, BLACK));
//   } else {                                 // flashing green
//     HAL_CHECK(rampup_rampdown(hardware));  // hardcoded to flash green
//   }
//   return hal::success();
// }
// hal::status rampup_rampdown(effect_hardware hardware)
// {
//   for (auto i = hardware.lights.begin(); i != hardware.lights.end(); i++) {
//     *i = GREEN;
//     hardware.driver->update(hardware.lights);
//     hal::delay(*hardware.clock, 10ms);
//   }
//   for (auto i = hardware.lights.rbegin(); i != hardware.lights.rend(); i++) {
//     *i = BLACK;
//     hardware.driver->update(hardware.lights);
//     hal::delay(*hardware.clock, 10ms);
//   }
// }
// hal::status light_change(effect_hardware hardware, rgb_brightness color)
// {  // red, blue, flashing green
//   for (auto i = hardware.lights.begin(); i != hardware.lights.end(); i++) {
//     *i = color;
//   }
//   hardware.driver->update(hardware.lights);
//   return hal::success();
// }
}  // namespace sjsu::drive
