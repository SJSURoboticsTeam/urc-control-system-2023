#pragma once
#include <libhal-util/steady_clock.hpp>
#include <libhal-lpc40/clock.hpp>
#include <libhal-soft/rc_servo.hpp>
#include <libhal-lpc40/pwm.hpp>
#include <libhal-pca/pca9685.hpp>
#include <libhal-lpc40/input_pin.hpp>
#include <libhal-util/units.hpp>

namespace sjsu::science {

    class revolver {
        private:
            int m_numVials = 12;
            hal::degrees m_clockwise = hal::degrees(0.0);
            hal::degrees m_counterclockwise = hal::degrees(180.0);
            hal::degrees m_stop = hal::degrees(360.0);
            std::chrono::milliseconds m_delay = std::chrono:milliseconds(5.0);

            hal::servo& revolver_servo_my;
            hal::input_pin& input_pin_my;
            hal::steady_clock& steady_clock_my;

            hal::status revolverState(hal::degrees rotationState);

        public:
            enum class positions{
                SAMPLE_DEPOSIT=0,
                MOLISCH_REAGENT,
                SULFURIC_ACID,
                BIURET_REAGENT,
                OBSERVATION
            };

            static hal::result<revolver> create();
                                                
            hal::status revolverMoveVials(int vial);
    };
}