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

namespace sjsu::science {
hal::status application(application_framework& p_resources)
{
  using namespace std::literals;

  auto& clock = *p_resources.steady_clock;
//   auto& console = *p_resources.terminal;

  auto clock_pin = HAL_CHECK(hal::lpc40::output_pin::get(1, 15));
  auto data_pin = HAL_CHECK(hal::lpc40::output_pin::get(1, 23));

  hal::sk9822<35> lights(clock_pin, data_pin, clock);
  lights.set_all(0x00,0x00,0x00, 0b000);
  lights.update(); 

  rgb_brightness ON, OFF;
  ON.set(0xff, 0x00, 0x00, 0b11111);
  OFF.set(0, 0, 0, 0);


  while (true) {
    // Print message
    hal::delay(clock, 50ms);
    lights.set_all(ON);
    lights.update(); 
    hal::delay(clock, 50ms);
    lights.set_all(OFF);
    lights.update(); 
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