#pragma once

#include <libhal-util/serial.hpp>
#include <libhal-util/units.hpp>

namespace sjsu::drive {

using namespace std::chrono_literals;
using namespace hal::literals;

struct drive_arguments
{
  hal::rpm speed = 0;
  hal::degrees angle = 0;
};

struct tri_wheel_router_arguments
{
  drive_arguments left{};
  drive_arguments right{};
  drive_arguments back{};
};

}  // namespace Drive
