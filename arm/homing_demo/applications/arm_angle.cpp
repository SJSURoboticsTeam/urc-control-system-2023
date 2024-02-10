#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>

#include "../../platform-implementations/home.hpp"
#include "application.hpp"

namespace sjsu::arm {

hal::status application(sjsu::arm::application_framework& p_framework)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;
  auto& shoulder_a = *p_framework.shoulder_accelerometer;
  auto& elbow_a = *p_framework.elbow_accelerometer;
  auto& rotunda_a = *p_framework.rotunda_accelerometer;
  auto& terminal = *p_framework.terminal;
  auto& clock = *p_framework.clock;

  hal::print(terminal, "INIT bb\n");

  while(true)
  {
    auto s = HAL_CHECK(shoulder_a.read());
    hal::print<128>(terminal, "shoulder: %fdeg\n", atan2_to_degrees(s.y, s.z));
    hal::delay(clock, 10ms);

    auto e = HAL_CHECK(elbow_a.read());
    hal::print<128>(terminal, "elbow: %fdeg\n", atan2_to_degrees(e.y, e.z));
    hal::delay(clock, 10ms);

    auto r = HAL_CHECK(rotunda_a.read());
    hal::print<128>(terminal, "rotunda: %fdeg\n\n", atan2_to_degrees(r.y, r.z));
    hal::delay(clock, 1s);
  }

  return hal::success();
}
}  // namespace sjsu::arm