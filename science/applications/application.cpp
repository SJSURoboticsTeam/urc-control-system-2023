#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>

#include "application.hpp"
namespace sjsu::science {

hal::status application(application_framework& p_framework)
{
  using namespace std::literals;
  
  auto& in_deionized_water_pump_pin = *p_framework.in_deionized_water_pump_pin;
  auto& in_sample_pump_pin = *p_framework.in_sample_pump_pin;
  auto& molisch_pump_pin = *p_framework.molisch_pump_pin;
  auto& in_sulfuric_acid_pin = *p_framework.in_sulfuric_acid_pin;
  auto& in_biuret_pump_pin = *p_framework.in_biuret_pump_pin;


  // auto& pwm_1_6 = *p_framework.pwm_1_6;
  // auto& pwm_1_5 = *p_framework.pwm_1_5;
  // auto& adc_4 = *p_framework.adc_4;
  // auto& adc_5 = *p_framework.adc_5;
  auto& mixing_servo = *p_framework.mixing_servo;

  auto& clock = *p_framework.steady_clock;
  auto& terminal = *p_framework.terminal;
  auto& mission_control = *(p_framework.mc);
  auto& in_pin2 = *p_framework.in_pin2;
  auto& steady_clock = *p_framework.steady_clock;
  // auto& revolver = *p_framework.revolver;

  // auto& can = *p_framework.can;
  // auto& i2c = *p_framework.i2c;

  // auto loop_count = 0;
 
  // auto& myScienceRobot = *p_framework.myScienceRobot;
  sjsu::science::mission_control::mc_commands commands;

  while (true) {
    // Print message
    hal::print(terminal, "Hello, World\n");
    auto timeout = hal::create_timeout(clock, 2s);
    commands = mission_control.get_command(timeout).value();
    commands.print(&terminal);    

  }
}
}  // namespace sjsu::science
