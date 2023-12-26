#include "../include/drc_speed_sensor.hpp"
#include <libhal-rmd/drc.hpp>

namespace sjsu::drive {

drc_speed_sensor::drc_speed_sensor(hal::rmd::drc& p_drc)
  : m_drc(&p_drc)
{
}

hal::result<speed_sensor::read_t> drc_speed_sensor::driver_read()
{
  HAL_CHECK(m_drc->feedback_request(hal::rmd::drc::read::status_2));

  return speed_sensor::read_t{ .speed = m_drc->feedback().speed() };
}

hal::result<drc_speed_sensor> make_speed_sensor(hal::rmd::drc& p_drc)
{
  return drc_speed_sensor(p_drc);
}
}  // namespace sjsu::drive