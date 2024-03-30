#include <libhal/error.hpp>

#include "applications/application.hpp"

sjsu::drive::application_framework hardware_map{};

int main()
{
  try {
    hardware_map = sjsu::drive::initialize_platform();
  } catch (...) {
    hal::halt();
  }

  sjsu::drive::application(hardware_map);

  return 0;
}