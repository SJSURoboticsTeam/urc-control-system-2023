#include <boost/ut.hpp>

#include <span>
#include <vector>

namespace project {
void project_test() {
  using namespace boost::ut;

  auto always_returns_2 = []() -> int { return 2; };

  "project::some_library()"_test = [always_returns_2]() {
    // Setup
    constexpr int expected = 2;

    // Exercise
    auto actual = always_returns_2();

    // Verify
    expect(that % expected == actual);
  };
};
}