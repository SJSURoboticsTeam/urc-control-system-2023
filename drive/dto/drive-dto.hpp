#pragma once

namespace Drive
{
    const char kResponseBodyFormat[] = "{\"HB\":%d,\"IO\":%d,\"WO\":%d,\"DM\":\"%c\",\"CMD\":[%d,%d]}\n";
    const char kGETRequestFormat[] = "drive?heartbeat_count=%d&is_operational=%d&wheel_orientation=%d&drive_mode=%c&speed=%d&angle=%d";

    struct drive_commands
    {
        char mode = 'D';
        int speed = 0;
        int angle = 0;
        int wheel_orientation = 0;
        int is_operational = 0;
        int heartbeat_count = 0;

        void Print()
        {
            printf(kResponseBodyFormat,heartbeat_count, is_operational, wheel_orientation, mode, speed, angle);
        }
    };

    struct motor_arguments
    {
        float speed = 0;
        float angle = 0;
    };

    struct leg_arguments
    {
        motor_arguments steer{motor_arguments{5, 0}};
        motor_arguments hub;
    };

    struct tri_wheel_router_arguments
    {
        leg_arguments left{};
        leg_arguments right{};
        leg_arguments back{};

        void Print()
        {
            // printf("Args\tLeg\tSpeed\tAngle\n");
            // printf("\tLeft\t%.1f\t%.1f\n", static_cast<double>(left.hub.speed), static_cast<double>(left.steer.angle));
            // printf("\tRight\t%.1f\t%.1f\n", static_cast<double>(right.hub.speed), static_cast<double>(right.steer.angle));
            // printf("\tBack\t%.1f\t%.1f\n", static_cast<double>(back.hub.speed), static_cast<double>(back.steer.angle));
        }
    };
} // sjsu::drive
