#include "../include/sk9822.hpp"
namespace sjsu::drive {

using namespace hal::literals;
using namespace std::chrono_literals;

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

}  // namespace sjsu::drive
