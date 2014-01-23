#include "../headers-shared/shared/LibC.hh"

int	LibC::atoi(char *str)
{
  return (::atoi(str));
}

void	*LibC::memset(void *data, int c, size_t n)
{
  return (::memset(data, c, n));
}

void	*LibC::memcpy(void *dest, const void * src, size_t n)
{
  return (::memcpy(dest, src, n));
}

int	LibC::mSleep(unsigned int milliseconds)
{
#ifdef	_WIN32
  MSLEEP(milliseconds);
  return (0);
#else
  return (MSLEEP(milliseconds));
#endif // _WIN32
}

time_t	LibC::time(time_t *timer)
{
  return (::time(timer));
}
