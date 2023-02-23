#include <libhal-lpc40xx/pwm.hpp>
#include <string_view>

using namespace std::chrono_literals;
using namespace hal::literals;

/*
take pwm generic as argument. composition class.
pwm safety protections so we don't blow shit up.
*/

class Safe_pwm{
    private:
        hal::hertz max_frequency;
        float max_duty_cycle;
    public:
        Safe_pwm(hal::hertz max_frequency, float max_duty_cycle, hal::pwm& pwmt) /*: hal::lpc40xx::pwm()*/{
            max_frequency = max_frequency;
            max_duty_cycle = max_duty_cycle;
        }
        float get_max_duty_cycle(){
            return max_duty_cycle;
        }
        hal::hertz get_frequency(){
            return max_frequency;
        }
};