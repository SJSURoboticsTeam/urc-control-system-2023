#pragma once

#include <libhal-util/units.hpp>

namespace sjsu::drive {

class speed_sensor 
{
    public:
    struct read_t {
        hal::rpm speed = 0;
    };

    [[nodiscard]] read_t read() 
    {
        return driver_read();
    }

    virtual ~speed_sensor() = default;

    private:
    virtual read_t driver_read() = 0;
};
}