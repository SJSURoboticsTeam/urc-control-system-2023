#include <libhal/error.hpp>

#include "applications/application.hpp"

sjsu::arm::application_framework hardware_map{};

int main()
{
  try {
    hardware_map = sjsu::arm::initialize_platform();
  } catch (...) {
    hal::halt();
  }

  sjsu::arm::application(hardware_map);

  return 0;
}