#pragma once

#include "../dto/drive-dto.hpp"
#include "steer-modes.hpp"

namespace Drive {
inline tri_wheel_router_arguments select_mode(drive_commands commands)
{
  switch (commands.mode) {
    case 'D':
      return SteerModes::drive_steering(commands);
      break;
    case 'S':
      return SteerModes::spin_steering(commands);
      break;
    case 'T':
      return SteerModes::translate_steering(commands);
      break;
    case 'H':
      // do nothing to get it into drive mode
      break;
    default:
      break;
  }
  return tri_wheel_router_arguments{};
}
}  // namespace Drive