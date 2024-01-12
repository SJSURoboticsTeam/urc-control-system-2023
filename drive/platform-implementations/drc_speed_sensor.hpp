#pragma once

#include "speed_sensor.hpp"
#include <libhal-rmd/drc.hpp>

namespace sjsu::drive {

class drc_speed_sensor;

hal::result<drc_speed_sensor> make_speed_sensor(hal::rmd::drc& p_drc);

class drc_speed_sensor : public speed_sensor {
private:
    drc_speed_sensor(hal::rmd::drc& p_drc);
    hal::result<speed_sensor::read_t> driver_read() override;
    friend hal::result<drc_speed_sensor> make_speed_sensor(hal::rmd::drc& p_drc);
    hal::rmd::drc* m_drc = nullptr;
};
}