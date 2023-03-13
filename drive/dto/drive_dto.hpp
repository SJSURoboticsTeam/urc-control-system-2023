#pragma once

#include <libhal-util/serial.hpp>

namespace Drive {
  using namespace std::chrono_literals;
  using namespace hal::literals;
static constexpr char kResponseBodyFormat[] =
  "{\"HB\":%d,\"IO\":%d,\"WO\":%d,\"DM\":\"%c\",\"CMD\":[%d,%d]}\n";

static constexpr char kGETRequestFormat[] =
  "drive?heartbeat_count=%d&is_operational=%d&wheel_orientation=%d&drive_mode=%"
  "c&speed=%d&angle=%d";

struct drive_commands
{
  char mode = 'D';
  int speed = 0;
  int angle = 0;
  int wheel_orientation = 0;
  int is_operational = 0;
  int heartbeat_count = 0;

  void print(hal::serial& terminal)
  {
    hal::print<100>(terminal,
                    kResponseBodyFormat,
                    heartbeat_count,
                    is_operational,
                    wheel_orientation,
                    mode,
                    speed,
                    angle);
  }
};

struct leg_arguments
{
  float speed = 0;
  float angle = 0;
};

struct tri_wheel_router_arguments
{
  leg_arguments left{};
  leg_arguments right{};
  leg_arguments back{};

  void print()
  {
    // printf("Args\tLeg\tSpeed\tAngle\n");
    // printf("\tLeft\t%.1f\t%.1f\n", static_cast<double>(left.hub.speed),
    // static_cast<double>(left.steer.angle)); printf("\tRight\t%.1f\t%.1f\n",
    // static_cast<double>(right.hub.speed),
    // static_cast<double>(right.steer.angle)); printf("\tBack\t%.1f\t%.1f\n",
    // static_cast<double>(back.hub.speed),
    // static_cast<double>(back.steer.angle));
  }
};
}  // namespace Drive
