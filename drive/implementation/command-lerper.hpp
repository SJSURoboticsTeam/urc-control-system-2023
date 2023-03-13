#pragma once

#include "../dto/drive-dto.hpp"

namespace Drive {
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
  static constexpr float speed_lerp = 0.4f;
  float previous_speed = 0;  // don't touch mode or wheel orientation logic
};
}  // namespace Drive