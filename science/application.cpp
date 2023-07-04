#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>

#include "application.hpp"

hal::status application(application_framework& p_framework)
{
  using namespace std::literals;

  auto& led = *p_framework.led;
  auto& clock = *p_framework.clock;
  auto& console = *p_framework.console;

  while (true) {
    // Print message
    hal::print(console, "Hello, World\n");

    // Toggle LED
    HAL_CHECK(led.level(true));
    hal::delay(clock, 500ms);

    HAL_CHECK(led.level(false));
    hal::delay(clock, 500ms);
  }
}