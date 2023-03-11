#pragma once
#include <cmath>
#include <algorithm>

#include "../dto/drive-dto.hpp"

namespace Drive
{
    class SteerModes
    {
    public:
        static constexpr float kLeftLegDriveOffset = 41;
        static constexpr float kRightLegDriveOffset = 200;
        static constexpr float kBackLegDriveOffset = 122;
        static constexpr float kLeftLegSpinOffset = 242;
        static constexpr float kRightLegSpinOffset = 0;
        static constexpr float kBackLegSpinOffset = 0;
        static constexpr float kLeftLegTranslateOffset = 0;

        static tri_wheel_router_arguments DriveSteering(drive_commands commands)
        {
            float outter_wheel_angle = 0, back_wheel_angle = 0;
            tri_wheel_router_arguments steer_arguments;

            if (commands.angle > 0)
            {
                outter_wheel_angle = static_cast<float>(-commands.angle);
                steer_arguments.left.steer.angle = outter_wheel_angle;
                steer_arguments.right.steer.angle = CalculateAckermann(outter_wheel_angle);
            }
            else if (commands.angle < 0)
            {
                outter_wheel_angle = static_cast<float>(-commands.angle);
                steer_arguments.right.steer.angle = outter_wheel_angle;
                steer_arguments.left.steer.angle = CalculateAckermann(outter_wheel_angle);
            }
            if (commands.angle == 0)
            {
                steer_arguments.back.steer.angle = 0;
                steer_arguments.left.steer.angle = 0;
                steer_arguments.right.steer.angle = 0;
            }
            else
            {
                outter_wheel_angle = -outter_wheel_angle;
                back_wheel_angle =
                    float(-.0474 + -1.93 * abs(int(outter_wheel_angle)) +
                          -.0813 * pow(abs(int(outter_wheel_angle)), 2) +
                          .000555 * pow(abs(int(outter_wheel_angle)), 3));
                back_wheel_angle = (outter_wheel_angle > 0) ? -back_wheel_angle : back_wheel_angle;
                steer_arguments.back.steer.angle = back_wheel_angle;
            }

            if (outter_wheel_angle > 0)
            {
                steer_arguments.right.hub.speed = -GetInnerWheelHubSpeed(static_cast<float>(commands.speed), outter_wheel_angle);
                steer_arguments.left.hub.speed = -static_cast<float>(commands.speed);
                steer_arguments.back.hub.speed = GetBackWheelHubSpeed(static_cast<float>(commands.speed), outter_wheel_angle);
            }

            else if (outter_wheel_angle < 0)
            {
                steer_arguments.left.hub.speed = -GetInnerWheelHubSpeed(static_cast<float>(commands.speed), outter_wheel_angle);
                steer_arguments.right.hub.speed = -static_cast<float>(commands.speed);
                steer_arguments.back.hub.speed = GetBackWheelHubSpeed(static_cast<float>(commands.speed), outter_wheel_angle);
            }
            else
            {
                steer_arguments.left.hub.speed = static_cast<float>(-commands.speed);
                steer_arguments.right.hub.speed = static_cast<float>(-commands.speed);
                steer_arguments.back.hub.speed = static_cast<float>(commands.speed);
            }

            // adding the offsets to put it into the base drive mode
            steer_arguments.left.steer.angle += kLeftLegDriveOffset;
            steer_arguments.right.steer.angle += kRightLegDriveOffset;
            steer_arguments.back.steer.angle += kBackLegDriveOffset;
            return steer_arguments;
        }

        static tri_wheel_router_arguments SpinSteering(drive_commands commands)
        {
            tri_wheel_router_arguments temp;
            temp.back.steer.angle = kBackLegSpinOffset;
            temp.left.steer.angle = kLeftLegSpinOffset;
            temp.right.steer.angle = kRightLegSpinOffset;
            temp.back.hub.speed = static_cast<float>(-commands.speed);
            temp.left.hub.speed = static_cast<float>(commands.speed);
            temp.right.hub.speed = static_cast<float>(-commands.speed);

            return temp;
        }

        static tri_wheel_router_arguments TranslateSteering(drive_commands commands)
        {
            tri_wheel_router_arguments steer_arguments;

            steer_arguments.left.steer.angle = (static_cast<float>(-commands.angle) + kLeftLegDriveOffset);
            steer_arguments.right.steer.angle = (static_cast<float>(-commands.angle) + kRightLegDriveOffset);
            steer_arguments.back.steer.angle = (static_cast<float>(-commands.angle) + kBackLegDriveOffset);

            steer_arguments.left.hub.speed = static_cast<float>(-commands.speed);
            steer_arguments.right.hub.speed = static_cast<float>(-commands.speed);
            steer_arguments.back.hub.speed = static_cast<float>(commands.speed);

            return steer_arguments;
        }

    private:
        /// Ackerman steering equation to compute outter wheel angle
        static float CalculateAckermann(float outter_wheel_angle)
        {
            float inner_wheel_angle =
                float(-.583 + 1.97 * abs(int(outter_wheel_angle)) +
                      -.224 * pow(abs(int(outter_wheel_angle)), 2) +
                      .0278 * pow(abs(int(outter_wheel_angle)), 3));
            return (outter_wheel_angle > 0) ? inner_wheel_angle : -inner_wheel_angle;
        }

        static float GetInnerWheelHubSpeed(float outter_wheel_speed, float outter_wheel_angle)
        {
            // clamps the inner wheel speed to be no faster then what will mess up the
            // correct ackerman velocity this clamp will then ensure the same for the
            // back wheel speed since its based on this angle
            float ratio = GetInnerWheelRadius(outter_wheel_angle) / 
            GetOutterWheelRadius(outter_wheel_angle);
            // std::clamp(inner_wheel_speed, -100 / ratio, 100 / ratio);
            return (outter_wheel_speed * ratio);
        }

        static float GetBackWheelHubSpeed(float outter_wheel_speed, float outter_wheel_angle)
        {
            float ratio = GetBackWheelRadius(outter_wheel_angle) /
                          GetOutterWheelRadius(outter_wheel_angle);
            // std::clamp(inner_wheel_speed * ratio, -100 / ratio, 100 / ratio);
            return (outter_wheel_speed * ratio);
        }

        static float GetInnerWheelRadius(float outter_wheel_angle)
        {
            return (15.0f * std::pow(std::abs(outter_wheel_angle), -.971f));
        }

        static float GetBackWheelRadius(float outter_wheel_angle)
        {
            return (11.6f * std::pow(std::abs(outter_wheel_angle), -.698f));
        }

        static float GetOutterWheelRadius(float outter_wheel_angle)
        {
            return (11.6f * std::pow(std::abs(outter_wheel_angle), -.616f));
        }
    };
} // sjsu::drive
