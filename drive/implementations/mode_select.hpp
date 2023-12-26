#pragma once

#include "../dto/drive.hpp"
#include "../include/mission_control.hpp"
#include "../include/mode_switcher.hpp"
#include "steer_modes.hpp"

namespace sjsu::drive {
inline tri_wheel_router_arguments select_mode(
  mission_control::mc_commands p_commands)
{
  switch (p_commands.mode) {
    case 'D':
      return drive_steering(p_commands);
      break;
    case 'S':
      return spin_steering(p_commands);
      break;
    case 'T':
      return translate_steering(p_commands);
      break;
    case 'H':
      // do nothing to get it into drive mode
      break;
    default:
      break;
  }
  return tri_wheel_router_arguments{};
}
}  // namespace sjsu::drive
