#pragma once

#include	"INetwork.hh"

class			Network
{
  unsigned short	_port;
  INetwork		*_moduleNetwork;

  Network(const Network& copy);
  Network&		operator=(const Network& other);

public:
  Network(unsigned short port);
  virtual ~Network();
  void			init();
  unsigned short	getPort() const;
  INetwork		*getModuleNetwork() const;
};
