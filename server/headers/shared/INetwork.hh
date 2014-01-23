#pragma once

#include <list>
#include <string>
#include <sys/types.h>
#include "triple.hpp"

class INetwork
{
public:
  virtual ~INetwork() {}
  virtual void									update() = 0;
  virtual void									send(unsigned int, const char *, size_t) = 0;
  virtual std::list<std::triple<unsigned int, char*, size_t> >&			receive() = 0;
  virtual std::list<std::pair<bool, unsigned int> >&				updateIdClient() = 0;
  virtual void									closeId(unsigned int) = 0;
  virtual std::string								getEndPointId(unsigned int) const = 0;
};
