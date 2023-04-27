#include "../../science/implementations/co2_sensor.hpp"
#include "../../science/implementations/mission_control_handler.hpp"
#include "../../science/implementations/mq4_methane_sensor.hpp"
#include "../../science/implementations/pressure_sensor_driver.hpp"
#include "../../science/implementations/state_machine.hpp"

#include <libhal-esp8266/at/socket.hpp>
#include <libhal-esp8266/at/wlan_client.hpp>
#include <libhal-esp8266/util.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal/adc.hpp>
#include <libhal/input_pin.hpp>

#include "../../hardware_map.hpp"
#include "../../science/dto/science_dto.hpp"

#include <string_view>

hal::status application(sjsu::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& pressure_adc = *p_map.adc_5;
  auto& methane_adc = *p_map.adc_4;
  auto& in_pin0 = *p_map.in_pin0;
  auto& revolver_spinner = *p_map.pwm_1_6;
  auto& seal_spinner = *p_map.pwm_1_5;
  auto& terminal = *p_map.terminal;
  auto pca9685 = HAL_CHECK(hal::pca::pca9685::create(*p_map.i2c, 0b100'0000));
  auto pca_pwm_0 = pca9685.get_pwm_channel<0>();
  auto pca_pwm_1 = pca9685.get_pwm_channel<1>();
  auto pca_pwm_2 = pca9685.get_pwm_channel<2>();
  auto& clock = *p_map.steady_clock;
  auto& esp = *p_map.esp;

  std::string response;
  int revolver_hall_value = 1;

  std::array<hal::byte, 8192> buffer{};
  static std::string_view get_request = "";

  HAL_CHECK(hal::write(terminal, "Starting program...\n"));
  auto wifi_result = hal::esp8266::at::wlan_client::create(
    esp,
    "SJSU Robotics 2.4GHz",
    "R0Bot1cs3250",
    hal::create_timeout(clock, 10s).value()); 

  while (!wifi_result) {
    wifi_result = hal::esp8266::at::wlan_client::create(
      esp,
      "SJSU Robotics 2.4GHz",
      "R0Bot1cs3250",
      hal::create_timeout(clock, 10s).value());
      if (!wifi_result) {
        HAL_CHECK(hal::write(terminal, "Failed to connect to wifi"));
      }
  }
  HAL_CHECK(hal::write(terminal, "ESP created!\n"));

  auto wifi = wifi_result.value();

  auto socket_result = hal::esp8266::at::socket::create(
    wifi,
    HAL_CHECK(hal::create_timeout(clock, 1s)), 
    {
      .type = hal::socket::type::tcp,
      .domain = "13.56.207.97",
      .port = "5000",
    });
    // this is for the web server hosted by nate: http://13.56.207.97:5000/science

  if (!socket_result) {
    HAL_CHECK(hal::write(terminal, "TCP Socket couldn't be established\n"));
    return socket_result.error();
  }

  auto socket = std::move(socket_result.value());
  HAL_CHECK(hal::write(terminal, "Server found"));

  // Constants
  static constexpr float MIN_SEAL_DUTY_CYCLE = 0.065f;
  static constexpr float MAX_SEAL_DUTY_CYCLE = 0.085f;

  // science::PumpPwmController air_pump(pca_pwm_0, 1000.0_Hz); // unknown
  // freqeuncy atm change when this is figured out science::PumpPwmController
  // dosing_pump(pca_pwm_1, 1000.0_Hz);
  science::pressure_sensor pressure(pressure_adc);
  science::mq4_methane_sensor methane(methane_adc);
  // auto carbon_dioxide = HAL_CHECK(science::Co2Sensor::create(*p_map.i2c,
  // clock));
  science::state_machine state_machine;

  science::science_commands mc_commands;
  science::science_data mc_data;

  HAL_CHECK(revolver_spinner.frequency(50.0_Hz));
  HAL_CHECK(hal::delay(clock, 10ms));
  HAL_CHECK(pca_pwm_0.frequency(1.50_kHz));
  HAL_CHECK(hal::delay(clock, 10ms));
  mc_commands.is_operational = 1;

  while (true) {
    // mc_commands = HAL_CHECK(mc_handler.ParseMissionControlData(response,
    // terminal));
    mc_data.pressure_level = HAL_CHECK(pressure.get_parsed_data());
    HAL_CHECK(hal::delay(clock, 5ms));
    mc_data.methane_level = HAL_CHECK(methane.get_parsed_data());
    HAL_CHECK(hal::delay(clock, 5ms));
    revolver_hall_value = HAL_CHECK(in_pin0.level()).state;
    HAL_CHECK(hal::delay(clock, 5ms));
    // mc_data.co2_level = HAL_CHECK(carbon_dioxide.read_co2());
    mc_data.pressure_level = 100;

    state_machine.run_machine(mc_data.status,
                              mc_commands,
                              mc_data.pressure_level,
                              revolver_hall_value,
                              terminal);
    mc_commands.state_step = 1;
    if (mc_data.status.move_revolver_status == science::status::in_progress) {
      HAL_CHECK(revolver_spinner.duty_cycle(0.076f));
      HAL_CHECK(hal::delay(clock, 5ms));
    } else if (mc_data.status.move_revolver_status ==
                 science::status::complete &&
               mc_data.status.seal_status == science::status::not_started) {
      HAL_CHECK(revolver_spinner.duty_cycle(0.075f));
      HAL_CHECK(hal::delay(clock, 5ms));
    } else if (mc_data.status.seal_status == science::status::in_progress) {
      HAL_CHECK(seal_spinner.duty_cycle(MAX_SEAL_DUTY_CYCLE));
      HAL_CHECK(hal::delay(clock, 5ms));
    } else if (mc_data.status.depressurize_status ==
               science::status::in_progress) {
      // start vacuum pump. Assume 40v input. Step down to 12v.
      // 12/40 = 0.30
      HAL_CHECK(pca_pwm_0.duty_cycle(0.40f));  // 12/30
      HAL_CHECK(hal::delay(clock, 5ms));
      HAL_CHECK(hal::delay(clock, 5ms));
      mc_data.pressure_level = 0;
    } else if (mc_data.status.depressurize_status ==
                 science::status::complete &&
               mc_data.status.inject_status == science::status::not_started) {
      // stop vacuum pump
      HAL_CHECK(pca_pwm_0.duty_cycle(0.00f));
      HAL_CHECK(hal::delay(clock, 5ms));
    } else if (mc_data.status.inject_status == science::status::in_progress) {
      // start injecting dosing pumps. Assume 40v input. Step down to 6v.
      // 6/40 = 0.15
      HAL_CHECK(pca_pwm_1.duty_cycle(0.15f));
      HAL_CHECK(pca_pwm_2.duty_cycle(0.15f));
      HAL_CHECK(hal::delay(clock, 5ms));
      HAL_CHECK(hal::delay(clock, 5s));
      mc_data.pressure_level = 100;
    } else if (mc_data.status.inject_status == science::status::complete) {
      // stop injecting dosing pumps
      HAL_CHECK(pca_pwm_1.duty_cycle(0.00f));
      HAL_CHECK(pca_pwm_2.duty_cycle(0.00f));
      HAL_CHECK(hal::delay(clock, 5ms));
      mc_commands.state_step = 2;
    } else if (mc_data.status.clear_status == science::status::in_progress) {
      // start vacuum pump. Assume 40v input. Step down to 12v.
      // 12/40 = 0.30
      HAL_CHECK(pca_pwm_0.duty_cycle(0.40f));  // 12/30
      HAL_CHECK(hal::delay(clock, 5ms));
      HAL_CHECK(hal::delay(clock, 5s));
      mc_data.pressure_level = 0;
    } else if (mc_data.status.clear_status == science::status::complete &&
               mc_data.status.unseal_status == science::status::not_started) {
      // stop vacuum pump
      HAL_CHECK(pca_pwm_0.duty_cycle(0.00f));
      HAL_CHECK(hal::delay(clock, 5ms));
    } else if (mc_data.status.unseal_status == science::status::in_progress) {
      HAL_CHECK(seal_spinner.duty_cycle(MIN_SEAL_DUTY_CYCLE));
      HAL_CHECK(hal::delay(clock, 2s));
    }
    response = science::create_GET_request_parameter_with_rover_status(mc_data);
    mc_data.status.Print(terminal);
    HAL_CHECK(hal::delay(clock, 10ms));
  }
  return hal::success();
}
