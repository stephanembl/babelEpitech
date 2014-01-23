#include "Client.hh"

Client::Client(unsigned int uid, Account *account)
  : _uid(uid), _ip("NONE"), _account(account), _hasHandShake(false), _connectionTime(LibC::time(NULL)), _lastPingTime(LibC::time(NULL))
{

}

Client::Client(const Client& copy)
  : _uid(copy._uid), _ip(copy._ip), _account(copy._account), _hasHandShake(copy._hasHandShake), _connectionTime(copy._connectionTime), _lastPingTime(copy._lastPingTime), _packet(copy._packet)
{

}

Client::~Client()
{

}

Client&		Client::operator=(const Client& other)
{
  if (&other != this)
    {
      this->_uid = other._uid;
      this->_ip = other._ip;
      this->_account = other._account;
      this->_hasHandShake = other._hasHandShake;
      this->_connectionTime = other._connectionTime;
      this->_lastPingTime = other._lastPingTime;
      this->_packet = other._packet;
    }
  return (*this);
}

unsigned int	Client::getUid() const
{
  return (this->_uid);
}

Account		*Client::getAccount() const
{
  return (this->_account);
}

void		Client::setAccount(Account *account)
{
  this->_account = account;
}

PacketFactory	&Client::getPacket()
{
  return (this->_packet);
}

void		Client::accountLogout()
{
  this->_account = NULL;
}

bool		Client::hasHandShake() const
{
  return (this->_hasHandShake);
}

void		Client::setHasHandShake(bool value)
{
  this->_hasHandShake = value;
}

time_t			Client::getConnectionTime() const
{
  return (this->_connectionTime);
}

time_t			Client::getLastPingTime() const
{
  return (this->_lastPingTime);
}

void			Client::setLastPingTime(time_t timer)
{
  this->_lastPingTime = timer;
}

void			Client::setIp(const std::string &ip)
{
  this->_ip = ip;
}

const std::string	&Client::getIp() const
{
  return (this->_ip);
}
