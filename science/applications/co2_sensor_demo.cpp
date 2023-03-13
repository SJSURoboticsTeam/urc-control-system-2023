#pragma once

#include <libhal-armcortex/dwt_counter.hpp>
#include <libhal-armcortex/startup.hpp>
#include <libhal-armcortex/system_control.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/units.hpp>

#include "../hardware_map.hpp"
#include "../implementations/co2_sensor.hpp"

using namespace hal::literals;
using namespace std::chrono_literals;

hal::status application(science::hardware_map& p_map)
{
  // configure drivers
  auto& carbon_dioxide_sensor = *p_map.carbon_dioxide_sensor;
  auto& steady_clock = p_map.steady_clock;

  auto co2_driver =
    HAL_CHECK(science::Co2Sensor::create(carbon_dioxide_sensor, steady_clock));

  while (true) {
    HAL_CHECK(hal::delay(*p_map.steady_clock, 500ms));
    auto co2 = HAL_CHECK(co2_driver.read_co2());
    hal::print<64>(*p_map.science_serial, "C02: %d\n", co2);
    HAL_CHECK(hal::delay(*p_map.steady_clock, 1000ms));
  }

  return hal::success();
}
