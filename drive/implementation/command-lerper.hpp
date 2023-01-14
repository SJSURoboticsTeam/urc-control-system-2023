#pragma once

#include "../dto/drive-dto.hpp"

namespace Drive
{
    class CommandLerper
    {
    public:
        drive_commands Lerp(drive_commands commands)
        {
            last_lerped_command_.speed = static_cast<int>(std::lerp(static_cast<float>(last_lerped_command_.speed), static_cast<float>(commands.speed), kSpeedLerp)); //get second eyes on this one
            commands.speed = last_lerped_command_.speed;
            return commands;
        }

    private:
        const float kSpeedLerp = 0.4f;
        const float kAngleLerp = 0.6f;
        drive_commands last_lerped_command_{}; // don't touch mode or wheel orientation logic
    };
}