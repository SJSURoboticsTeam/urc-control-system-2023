#pragma once

#include <libhal-util/serial.hpp>

namespace sjsu::drive{

static constexpr char kResponseBodyFormat[] =
  "{\"HB\":%d,\"IO\":%d,\"WO\":%d,\"DM\":\"%c\",\"CMD\":[%d,%d]}\n";

static constexpr char kGETRequestFormat[] =
  "drive?heartbeat_count=%d&is_operational=%d&wheel_orientation=%d&drive_mode=%"
  "c&speed=%d&angle=%d";

class mission_control {
    public:
  struct mc_commands
  {
    char mode = 'D';
    int speed = 0;
    int angle = 0;
    int wheel_orientation = 0;
    int is_operational = 0;
    int heartbeat_count = 0;
  };

    virtual hal::result<mc_commands> get_commands() = 0;
    virtual hal::status send_commands() = 0;
};

}
