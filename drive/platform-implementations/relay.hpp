#pragma once

#include <libhal/output_pin.hpp>

namespace sjsu::drive
{

class relay : public hal::output_pin {
public:
    static hal::result<relay> create(hal::output_pin& p_toggle) {
        return relay(p_toggle);
    }

    void turn_on() {

    }



private:
    level_t level;
    

    relay(hal::output_pin& p_toggle) : m_toggle(&p_toggle)
    {
    }

    hal::output_pin* m_toggle = nullptr;
    
    status driver_configure(const settings& p_settings) override {
        return hal::success();
    }
    result<set_level_t> driver_level(bool p_high) override {

    }
    result<level_t> driver_level() override {

    }


}


}