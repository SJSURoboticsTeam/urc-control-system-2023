#include <libhal-esp8266/util.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal-util/map.hpp>
#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../hardware_mapp.hpp"

#include <string>
#include <string_view>

hal::status application(science::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& console = *p_map.terminal;
  auto& steady_clock = *p_map.steady_clock;
  auto& i2c = *p_map.i2c;
}