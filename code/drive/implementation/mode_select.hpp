#pragma once

#include "../dto/drive_dto.hpp"
#include "mode_switcher.hpp"
#include "steer_modes.hpp"

namespace Drive {
inline tri_wheel_router_arguments select_mode(drive_commands commands)
{
  switch (commands.mode) {
    case 'D':
      return drive_steering(commands);
      break;
    case 'S':
      return spin_steering(commands);
      break;
    case 'T':
      return translate_steering(commands);
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