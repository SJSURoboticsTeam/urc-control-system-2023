#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>

#include "application.hpp"

hal::status application(application_framework& p_framework)
{
  using namespace std::literals;
  // Use items from p_framework here

  while (true) {
    // call mission control interface here and other business logic to control the arm
  }
}
