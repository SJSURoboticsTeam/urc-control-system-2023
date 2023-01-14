#pragma once

#include "../dto/drive-dto.hpp"
#include "steer-modes.hpp"

namespace Drive
{
    class ModeSelect
    {
    public:
        static tri_wheel_router_arguments SelectMode(drive_commands commands)
        {
            switch (commands.mode)
            {
            case 'D':
                return SteerModes::DriveSteering(commands);
                break;
            case 'S':
                return SteerModes::SpinSteering(commands);
                break;
            case 'T':
                return SteerModes::TranslateSteering(commands);
                break;
            default:
                return tri_wheel_router_arguments{};
                break;
            }
        }

    private:
    };
}