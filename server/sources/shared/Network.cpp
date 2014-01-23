#include "Network.hh"
#include "TCPBoost.hpp"

Network::Network(unsigned short port)
  : _port(port)
{

}

Network::~Network()
{
  delete this->_moduleNetwork;
}

void	Network::init()
{
  this->_moduleNetwork = new TCPBoost(this->_port);
}

unsigned short	Network::getPort() const
{
  return (this->_port);
}

INetwork	*Network::getModuleNetwork() const
{
  return (this->_moduleNetwork);
}
