#pragma once

namespace sjsu::drive {

class relay 
{
    public:
    struct set_status_t 
    {};

    struct status_t {
        bool status = false;
    };

    [[nodiscard]] hal::result<status_t> status()
    {
        return driver_status(); 
    }

    [[nodiscard]] hal::result<set_status_t> toggle(bool p_toggle)
    {
        return driver_toggle(p_toggle); 
    }

    virtual ~relay() = default;

    private:
    virtual hal::result<status_t> driver_status() = 0;
    virtual hal::result<set_status_t> driver_toggle(bool p_toggle) = 0;
};
}


// static hal::result<relay> create(hal::output_pin& p_toggle) {
    //     return relay(p_toggle);
    // }