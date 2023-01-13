#include <libhal/config.hpp>

namespace project {
extern void project_test();
}

int main()
{
  static_assert(
    hal::is_a_test(),
    "The project platform is NOT a test build. Ensure that the "
    "\"libhal.tweaks.hpp\" file exists, can be found in the build system's "
    "include path, and has the platform config option set to \"test\".");

    project::project_test();
}
