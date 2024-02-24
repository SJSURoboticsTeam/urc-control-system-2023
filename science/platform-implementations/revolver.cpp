#pragma once
#include "revolver.hpp"
#include <libhal-util/steady_clock.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-soft/rc_servo.hpp>
#include <libhal-lpc40/pwm.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal-lpc40/input_pin.hpp>
#include <libhal-util/units.hpp>
namespace sjsu::science {

    revolver::revolver(hal::servo& p_servo, hal::input_pin& p_input_pin, hal::steady_clock& p_steady_clock)
    {
        revolver::create(p_servo, p_input_pin, p_steady_clock);
    }
    
    hal::result<revolver> revolver::create(hal::servo& p_servo, hal::input_pin& p_input_pin, hal::steady_clock& p_steady_clock) 
    {
        revolver_servo_my = p_servo;
        input_pin_my = p_input_pin;
        steady_clock_my = p_steady_clock;
    }

    hal::status revolver::revolverMove(hal::degrees rotation) 
    {
        HAL_CHECK(revolver_servo_my.position(rotation));
    }

    hal::status revolver::revolverStop()
    {
        HAL_CHECK(revolver_servo_my.position(hal::degrees(360.0)));
    }

    hal::status revolver::revolverMoveVials(int vial) 
    {
        int count = 0;
        hal::degrees rotation = hal::degrees(0.0); //clockwise

        if (vial < 0)
        {
            vial = -vial; 
            rotation = hal::degrees(180.0); //counterclockwise
        }

        if (vial <= m_numVials && vial != 0)
        {
            revolverMove(rotation);

            while (count < vial) 
            {
                bool state = input_pin_my.level().value().state;

                hal::delay(steady_clock_my, std::chrono::milliseconds(5.0));

                bool stateDelay = input_pin_my.level().value().state;

                if (state != stateDelay)
                {
                    count = count + 1;
                }
            }

            revolverStop();
        }
        
    }     

}