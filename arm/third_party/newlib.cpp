#include <sys/stat.h>
#include <unistd.h>

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>

extern "C"
{

  int _getpid_r()
  {
    return 1;
  }

  int _kill_r(int, int)
  {
    return -1;
  }

  int _fstat_r([[maybe_unused]] int file, struct stat* status)
  {
    status->st_mode = S_IFCHR;
    return 0;
  }

  int _write_r([[maybe_unused]] int file,
               [[maybe_unused]] const char* ptr,
               int length)
  {
    return length;
  }

  int _read_r([[maybe_unused]] FILE* file,
              [[maybe_unused]] char* ptr,
              int length)
  {
    return length;
  }

  int _lseek_r([[maybe_unused]] int file,
               [[maybe_unused]] int ptr,
               [[maybe_unused]] int dir)
  {
    return 0;
  }

  int _close_r([[maybe_unused]] int file)
  {
    return -1;
  }

  int _isatty_r([[maybe_unused]] int file)
  {
    return 1;
  }
}
