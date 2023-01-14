#pragma once

// changed -> libhal

#include <liblpc40xx/input_pin.hpp>
#include <libhal-util/serial.hpp>
#include <librmd/drc.hpp>
#include <libhal-util/units.hpp>

#include "../soft-driver/rmd-encoder.hpp"
#include "../dto/drive-dto.hpp"
#include "../dto/motor-feedback-dto.hpp"

namespace Drive
{
    class TriWheelRouter
    {
    public:
    using namespace hal::literals;
        struct leg
        {
            leg(hal::rmd& steer, hal::rmd& drive, hal::lpc40xx::input_pin& magnet) : steer_motor_(steer), drive_motor_(drive), magnet_(magnet)
            {
            }
            hal::rmd::drc& steer_motor_;
            hal::rmd::drc& drive_motor_;
            hal::lpc40xx::input_pin& magnet_;
            int16_t wheel_offset_=0;
        };

        TriWheelRouter(leg right, leg left, leg back) : left_(left), back_(back), right_(right)
        {
        }

        tri_wheel_router_arguments SetLegArguments(tri_wheel_router_arguments tri_wheel_arguments)
        {
            left_.steer_motor_.position_control(hal::angle(-tri_wheel_arguments.left.steer.angle + left_.wheel_offset_),
                                        hal::rpm(tri_wheel_arguments.left.steer.speed));
            left_.drive_motor_.velocity_control(hal::rpm(tri_wheel_arguments.left.hub.speed));

            right_.steer_motor_.position_control(hal::angle(-tri_wheel_arguments.right.steer.angle + right_.wheel_offset_),
                                         hal::rpm(tri_wheel_arguments.right.steer.speed));
            right_.drive_motor_.velocity_control(hal::rpm(tri_wheel_arguments.right.hub.speed));

            back_.steer_motor_.position_control(hal::angle(-tri_wheel_arguments.back.steer.angle + back_.wheel_offset_),
                                        hal::rpm(tri_wheel_arguments.back.steer.speed));
            back_.drive_motor_.velocity_control(hal::rpm(tri_wheel_arguments.back.hub.speed));

            tri_wheel_arguments_ = tri_wheel_arguments;
            return tri_wheel_arguments_;
        }

        tri_wheel_router_arguments GetTriWheelRouterArguments() const
        {
            return tri_wheel_arguments_;
        }

        /// At the moment, homing is where the legs turn on so we just calculate the initial encoder positions. ***Must be called in main
        void HomeLegs()
        {
            motor_feedback angle_verification;

            while (WheelNotZeroDoThis(left_) | WheelNotZeroDoThis(right_) | WheelNotZeroDoThis(back_))
            {
                //This loops until all of the wheels are zeroed and/or homed
            }

            bool leftNotHome = left_.magnet_.level(), rightNotHome = right_.magnet_.level(), backNotHome = back_.magnet_.level();

            while ((!leftNotHome && WheelNotNeg60DoThis(left_)) | (!rightNotHome && WheelNotNeg60DoThis(right_)) | (!backNotHome && WheelNotNeg60DoThis(back_)))
            {
                //This loop moves wheels that were prematurely homed away from their current position
            }

            while (WheelNotHomeDoThis(left_) | WheelNotHomeDoThis(right_) | WheelNotHomeDoThis(back_))
            {
                HAL_CHECK(hal::write(*p_map.terminal, "HomingPins L = %d\t R = %d\t B = %d", left_.magnet_->level(), right_.magnet_.level(), back_.magnet_.level()));
                HAL_CHECK(hal::write(*p_map.terminal, "b = %d\tr = %d\tl = %d", back_.wheel_offset_, right_.wheel_offset_, left_.wheel_offset_));
                angle_verification = GetMotorFeedback();
                while (angle_verification.left_steer_speed != 0_rpm | angle_verification.right_steer_speed != 0_rpm | angle_verification.back_steer_speed != 0_rpm)
                {
                    angle_verification = GetMotorFeedback();
                }
            }
        }

        motor_feedback GetMotorFeedback()
        {
            motor_feedback motor_speeds;
            // Creating this enum from the drc class allows us to read all data from the rmd when it is passed into the feedback_request function
            hal::rmd::drc::read read_commands;

            HAL_CHECK(left_.steer_motor_.feedback_request(read_commands));
            HAL_CHECK(right_.steer_motor_.feedback_request(read_commands));
            HAL_CHECK(left_.steer_motor_.feedback_request(read_commands));

            motor_speeds.left_steer_speed = left_.steer_motor_.feedback().speed();
            motor_speeds.right_steer_speed = right_.steer_motor_.feedback().speed();
            motor_speeds.back_steer_speed = back_.steer_motor_.feedback().speed();
            return motor_speeds;
        }

    private:
        bool WheelNotZeroDoThis(leg &leg_)
        {
            // This leg is NOT at zero
            if ((Drive::RmdEncoder::CalcEncoderPositions(leg_.steer_motor_, 360.0f) >= 0.01f) || Drive::RmdEncoder::CalcEncoderPositions(leg_.steer_motor_, 360.0f) <= -0.01f)
            {
                leg_.steer_motor_.position_control(0_deg, 2_rpm);
                // This wheel is NOT at zero
                return true;
            }
            else
            {
                // This wheel is at zero
                return false;
            }
        }

        
        bool WheelNotNeg60DoThis(leg &leg_)
        {
            leg_.wheel_offset_ = -60;
            hal::rmd::drc::read read_commands;
            leg_.steer_motor_.position_control(-60_deg, 2_rpm);

            HAL_CHECK(left_.steer_motor_.feedback_request(read_commands));

            if (left_.steer_motor_.feedback().speed() != 0_rpm)
            {
                // This wheel is NOT at -60
                return true;
            }
            else
            {
                // This wheel is at -60
                return false;
            }
        }

        bool WheelNotHomeDoThis (leg& leg_) {
            // level returns true if it is high, and the magnet is high when it is not 
            bool not_homed = leg_.magnet_.level()
            if (not_homed)
            {
                leg_.wheel_offset_++;
                leg_.steer_motor_.position_control(hal::angle(leg_.wheel_offset_), 2_rpm);
                return true;
            }
            else 
            {
            return false;
            }
        }

        // member variables

        leg left_;
        leg back_;
        leg right_;
        tri_wheel_router_arguments tri_wheel_arguments_;
    };
}