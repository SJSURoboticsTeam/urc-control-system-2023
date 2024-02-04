#pragma once
#include "../include/mission_control.hpp"
#include <array>
#include <libhal-util/steady_clock.hpp>
#include <libhal/output_pin.hpp>
#include <libhal/steady_clock.hpp>
#include <libhal/units.hpp>
#include <span>

namespace sjsu::drive {

struct rgb_brightness
{
  hal::byte r = 0, g = 0, b = 0;
  hal::byte brightness = 0;

  constexpr rgb_brightness(){};
  constexpr rgb_brightness(hal::byte p_r,
                           hal::byte p_g,
                           hal::byte p_b,
                           hal::byte p_brightness)
  {
    r = p_r;
    g = p_g;
    b = p_b;
    brightness = p_brightness;
  }
  constexpr void set(hal::byte p_r,
                     hal::byte p_g,
                     hal::byte p_b,
                     hal::byte p_brightness)
  {
    r = p_r;
    g = p_g;
    b = p_b;
    brightness = p_brightness;
  }
};

using namespace std::chrono_literals;
namespace colors {
constexpr rgb_brightness RED = rgb_brightness(0xff, 0x00, 0x00, 0b11111);
constexpr rgb_brightness GREEN = rgb_brightness(0x00, 0xff, 0x00, 0b11111);
constexpr rgb_brightness BLUE = rgb_brightness(0x00, 0x00, 0xff, 0b11111);
constexpr rgb_brightness WHITE = rgb_brightness(0xff, 0xff, 0xff, 0b11111);
constexpr rgb_brightness BLACK = rgb_brightness(0x00, 0x00, 0x00, 0x00);
};  // namespace colors

template<std::size_t n_leds>
using light_strip = std::array<sjsu::drive::rgb_brightness, n_leds>;

using light_strip_view = std::span<sjsu::drive::rgb_brightness>;

void set_all(sjsu::drive::light_strip_view lights,
             const hal::byte r,
             const hal::byte g,
             const hal::byte b,
             const hal::byte brightness);
void set_all(sjsu::drive::light_strip_view lights, const rgb_brightness value);

class sk9822
{
public:
  constexpr static auto period = 14ns;
  constexpr static auto half_period = period / 2;

  sk9822(hal::output_pin& clock_pin,
         hal::output_pin& data_pin,
         hal::steady_clock& clock);
  hal::status update(sjsu::drive::light_strip_view lights);
  // overriding brightness will ignore the default values for brightness passed into the struct 
  hal::status update(sjsu::drive::light_strip_view lights, hal::byte overriding_brightness);

private:
  hal::output_pin *clock_pin, *data_pin;
  hal::steady_clock* clock;

  hal::status send_byte(hal::byte value);
};

}  // namespace sjsu::drive
