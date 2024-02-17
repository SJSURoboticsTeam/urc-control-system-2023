#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>

#include "application.hpp"
namespace sjsu::science {

hal::status application(application_framework& p_framework)
{
  using namespace std::literals;

  auto& clock = *p_framework.steady_clock;
  auto& mission_control = *(p_framework.mc);
  auto& console = *p_framework.terminal;
  auto& myScienceRobot = *p_framework.myScienceRobot;
  sjsu::science::mission_control::mc_commands commands;



  while (true) {
    // Print message
    hal::print(console, "Hello, World\n");
    

  }
}
}  // namespace sjsu::science
