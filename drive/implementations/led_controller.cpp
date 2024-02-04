#include "../include/sk9822.hpp"
#include "../applications/application.hpp"
namespace sjsu::drive{
class led_controller {
public:
led_controller(application_framework& p_framework)
    :m_framework(p_framework)
{
}
hal::status light_change(effect_hardware& hardware, rgb_brightness color)
{  // red, blue, flashing green

  for (auto i = hardware.lights.begin(); i != hardware.lights.end(); i++) {
    *i = color;
  }
  hardware.driver->update(hardware.lights, 0b0111);
  return hal::success();
}

hal::status rampup_rampdown(effect_hardware& hardware)
{
  light_change(hardware, colors::GREEN);
  hal::delay(*hardware.clock, 50ms);
  light_change(hardware, colors::BLACK);
  hal::delay(*hardware.clock, 50ms);

  return hal::success();
}

hal::status control_leds(mission_control::mc_commands commands) {
    auto led_hardware = *m_framework.led_strip;
    auto& terminal = *m_framework.terminal;
    auto& clock = *m_framework.clock;

    hal::print<512>(terminal, "%d\n", commands.led_status);
    if (commands.led_status == 1) {  // red - autonomous driving
      HAL_CHECK(light_change(led_hardware, colors::RED));
    } else if (commands.led_status == 2) {  // blue - manually driving
      HAL_CHECK(light_change(led_hardware, colors::BLUE));
    } else {                                     // flashing green
      HAL_CHECK(rampup_rampdown(led_hardware));  // hardcoded to flash green
    }
    return hal::success();
}
private:
    application_framework& m_framework;
};
}
