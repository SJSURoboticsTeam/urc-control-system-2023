#include <cinttypes>

#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>
#include <libhal/can.hpp>
#include <librmd/drc.hpp>

#include "../hardware_map.hpp"

bool led_state = false;

// [[nodiscard]] hal::status print_feedback_and_delay(
//   hal::rmd::drc& p_drc,
//   hal::serial& p_serial,
//   hal::steady_clock& p_steady_clock,
//   hal::time_duration p_duration) noexcept
// {
//   std::array<char, 256> buffer;
//   HAL_CHECK(hal::delay(p_steady_clock, p_duration / 2));
//   p_drc.feedback_request(hal::rmd::drc::read::status_1_and_error_flags);
//   p_drc.feedback_request(hal::rmd::drc::read::status_2);
//   HAL_CHECK(hal::delay(p_steady_clock, p_duration / 2));
//   int count =
//     std::snprintf(buffer.data(),
//                   buffer.size(),
//                   "raw_current = %" PRId16 "\n"
//                   "raw_voltage = %" PRId16 "\n"
//                   "raw_motor_temperature = %" PRId16 "\n"
//                   "raw_speed = %" PRId16 "\n"
//                   "over_temperature_protection_tripped = %d\n"
//                   "over_voltage_protection_tripped = %d\n"
//                   "encoder = %" PRId16 "\n\n",
//                   p_drc.feedback().raw_current,
//                   p_drc.feedback().raw_volts,
//                   p_drc.feedback().raw_motor_temperature,
//                   p_drc.feedback().raw_speed,
//                   p_drc.feedback().over_temperature_protection_tripped,
//                   p_drc.feedback().over_voltage_protection_tripped,
//                   p_drc.feedback().encoder);

//   HAL_CHECK(hal::write(
//     p_serial, hal::as_bytes(buffer).subspan(0, static_cast<size_t>(count))));

//   led_state = !led_state;
//   HAL_CHECK(p_map.out_pin->level(led_state));

//   return hal::success();
// }

hal::status application(drive::hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;
  auto& can = *p_map.can;
  auto& terminal = *p_map.terminal;

  HAL_CHECK(hal::write(terminal, "Starting RMD Demo\n"));
  // delay for 1 second
  HAL_CHECK(hal::delay(*p_map.steady_clock, 1s));

  auto router = HAL_CHECK(hal::can_router::create(can));
  auto drc = HAL_CHECK(hal::rmd::drc::create(router, 8.0f, 0x141));

  HAL_CHECK(hal::delay(*p_map.steady_clock, 1s));
  HAL_CHECK(hal::write(terminal, "goooo\n"));

  while (true) {
    drc.velocity_control(30.0_rpm);
    HAL_CHECK(hal::delay(*p_map.steady_clock, 3s));
    HAL_CHECK(hal::write(terminal, "goooo2\n"));

    drc.velocity_control(-30.0_rpm);
    HAL_CHECK(hal::delay(*p_map.steady_clock, 3s));

    drc.system_control(hal::rmd::drc::system::off);
    HAL_CHECK(hal::delay(*p_map.steady_clock, 3s));

    drc.system_control(hal::rmd::drc::system::running);
    HAL_CHECK(hal::delay(*p_map.steady_clock, 3s));
  }

  return hal::success();
}