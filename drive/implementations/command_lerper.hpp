#pragma once

#include "../dto/drive.hpp"

namespace sjsu::drive {
class command_lerper
{
public:
  int lerp(int speed)
  {
    previous_speed =
      static_cast<int>(std::lerp(static_cast<float>(previous_speed),
                                 static_cast<float>(speed),
                                 speed_lerp));
    return previous_speed;
  }

private:
  static constexpr float speed_lerp = 0.1f;
  float previous_speed = 0;  // don't touch mode or wheel orientation logic
};
}  // namespace Drive