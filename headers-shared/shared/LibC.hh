#pragma once

#include <cstdlib>
#include <cstring>
#include <time.h>

#ifdef	_WIN32
# define NOMINMAX
# include <windows.h>
# define	MSLEEP(x)	Sleep(x)
#else
# include <unistd.h>
# define	MSLEEP(x)	usleep(x * 1000)
#endif // !WIN32

class		LibC
{
public:
  virtual ~LibC() {};
  static int	atoi(char *str);
  static void	*memset(void *data, int c, size_t n);
  static void	*memcpy(void *dst, const void *src, size_t n);
  static int	mSleep(unsigned int milliseconds);
  static time_t	time(time_t *timer);
};
