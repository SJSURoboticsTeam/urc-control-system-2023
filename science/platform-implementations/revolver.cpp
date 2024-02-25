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
        : revolver_servo_my(p_servo), input_pin_my(p_input_pin), steady_clock_my(p_steady_clock)
    {
    }

    hal::result<revolver> revolver::create(hal::servo& p_servo, hal::input_pin& p_input_pin, hal::steady_clock& p_steady_clock) 
    {
        return hal::result<revolver>(p_servo, p_input_pin, p_steady_clock);
    }

    hal::status revolver::revolverState(hal::degrees rotationState) 
    {
        HAL_CHECK(revolver_servo_my.position(rotationState));
    }

    hal::status revolver::revolverMoveVials(int vial) 
    {
        int count = 0;
        bool hallState;
        bool hallStateDelayed;
        
        if (vial != 0 && std::abs(vial) <= m_numVials)
        {
            Direction direction = (vial > 0) ? m_clockwise : m_counterclockwise;
            revolverState(direction);
        }

        while (count < std::abs(vial)) 
        {
            hallState = input_pin_my.level().value().state;
            hal::delay(steady_clock_my, m_delay);
            hallStateDelayed = input_pin_my.level().value().state;
            
            if (hallState != hallStateDelayed) 
            {
                ++count;
            }
        }

        revolverState(m_stop);
    }
}     