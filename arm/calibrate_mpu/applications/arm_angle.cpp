#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/steady_clock.hpp>

#include "../../platform-implementations/home.hpp"
#include "application.hpp"

namespace sjsu::arm {
// hal::accelerometer::read_t apply_offsets(hal::accelerometer::read_t reading, float x, float y, float z)  {
//   reading.x += x;
//   reading.y += y;
//   reading.z += z;
//   return reading;
// }

hal::status application(sjsu::arm::application_framework& p_framework)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;
  auto& shoulder_a = *p_framework.shoulder_accelerometer;
  auto& elbow_a = *p_framework.elbow_accelerometer;
  auto& rotunda_a = *p_framework.rotunda_accelerometer;
  auto& terminal = *p_framework.terminal;
  auto& clock = *p_framework.clock;

  hal::print(terminal, "calibrating\n");

  int N = 1000;
  hal::accelerometer::read_t s_sum = { 0 };
  hal::accelerometer::read_t e_sum = { 0 };
  hal::accelerometer::read_t r_sum = { 0 };
  for(int i = 0; i < N; i ++) {
    auto s = HAL_CHECK(shoulder_a.read());
    // s = apply_offsets(s, -0.054775, 2.027401, 0.098534);
    s_sum.x += s.x;
    s_sum.y += s.y;
    s_sum.z += s.z;
    auto e = HAL_CHECK(elbow_a.read());
    // e = apply_offsets(e, 0.092266, 0.046047, 1.933188);
    e_sum.x += e.x;
    e_sum.y += e.y;
    e_sum.z += e.z;
    auto r = HAL_CHECK(rotunda_a.read());
    // r = apply_offsets(r, -0.000677, -0.046465, 1.859087);
    r_sum.x += r.x;
    r_sum.y += r.y;
    r_sum.z += r.z;
    // hal::print<512>(terminal, "Shoulder: %f, %f, %f\n", s.x, s.y, s.z);
    // hal::print<512>(terminal, "Elbox %f, %f, %f\n", e.x, e.y, e.z);
    // hal::print<512>(terminal, "Rotunda: %f, %f, %f\n", r.x, r.y, r.z);
    if(i% 10 == 9) {
      hal::print<512>(terminal, "%d\n", i);
    }
    hal::delay(clock, 1ms);
  }
  s_sum.x /= N;
  s_sum.y /= N;
  // s_sum.y -= -1;
  s_sum.z /= N;

  e_sum.x /= N;
  e_sum.y /= N;
  e_sum.z /= N;
  // e_sum.z -= -1;

  r_sum.x /= N;
  r_sum.y /= N;
  r_sum.z /= N;
  // r_sum.z -= -1;
  hal::print(terminal, "------------- Average Reading --------------\n");
  hal::print<512>(terminal, "Shoulder: %f, %f, %f\n", s_sum.x, s_sum.y, s_sum.z);
  hal::print<512>(terminal, "Elbox %f, %f, %f\n", e_sum.x, e_sum.y, e_sum.z);
  hal::print<512>(terminal, "Rotunda: %f, %f, %f\n", r_sum.x, r_sum.y, r_sum.z);


  // while(true);
  while(true)
  {
    auto s = HAL_CHECK(shoulder_a.read());
    // s = apply_offsets(s, -0.054775, 2.027401, 0.098534); // SHOULDER OFFSET
    hal::print<128>(terminal, "shoulder: %fdeg\n", atan2_to_degrees(s.y, s.z));
    hal::delay(clock, 10ms);

    auto e = HAL_CHECK(elbow_a.read());
    // e = apply_offsets(e, 0.092266, 0.046047, 1.933188); // ELBOW OFFSET
    hal::print<128>(terminal, "elbow: %fdeg\n", atan2_to_degrees(e.y, e.z));
    hal::delay(clock, 10ms);

    auto r = HAL_CHECK(rotunda_a.read());
    // r = apply_offsets(r, -0.000677, -0.046465, 1.859087); // ROTUNDA OFFSET
    hal::print<128>(terminal, "rotunda: %fdeg\n\n", atan2_to_degrees(r.y, r.z));
    hal::delay(clock, 1s);
  }

  return hal::success();
}
}  // namespace sjsu::arm