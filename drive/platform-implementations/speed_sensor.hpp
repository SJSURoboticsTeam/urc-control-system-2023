#pragma once

#include <libhal-util/units.hpp>

namespace sjsu::drive {

class speed_sensor 
{
    public:
    struct read_t {
        hal::rpm speed = 0;
    };

    [[nodiscard]] hal::result<read_t> read() 
    {
        return driver_read();
    }

    virtual ~speed_sensor() = default;

    private:
    virtual hal::result<read_t> driver_read() = 0;
};
}