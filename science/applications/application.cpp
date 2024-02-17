#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>

#include "application.hpp"
namespace sjsu::science {

hal::status application(application_framework& p_framework)
{
  using namespace std::literals;
  
  auto& in_pin0 = *p_framework.in_pin0;
  auto& in_pin1 = *p_framework.in_pin1;
  auto& in_pin2 = *p_framework.in_pin2;
  auto& pwm_1_6 = *p_framework.pwm_1_6;
  auto& pwm_1_5 = *p_framework.pwm_1_5;
  auto& adc_4 = *p_framework.adc_4;
  auto& adc_5 = *p_framework.adc_5;

  auto& clock = *p_framework.steady_clock;
  auto& terminal = *p_framework.terminal;
  auto& mission_control = *(p_framework.mc);
  auto& can = *p_framework.can;
  auto& i2c = *p_framework.i2c;
  auto loop_count = 0;
 
  // auto& myScienceRobot = *p_framework.myScienceRobot;
  sjsu::science::mission_control::mc_commands commands;



  while (true) {
    // Print message
    hal::print(terminal, "Hello, World\n");
    if (loop_count == 10) {
      auto timeout = hal::create_timeout(clock, 100ms);
      commands = mission_control.get_command(timeout).value();
      loop_count = 0;
    }
    loop_count++;
    // commands = validate_commands(commands);
    

  }
}
}  // namespace sjsu::science
