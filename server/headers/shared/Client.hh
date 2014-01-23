#pragma once

#include "LibC.hh"
#include "PacketFactory.h"

class		Account;

class		Client
{
  unsigned int		_uid;
  std::string		_ip;
  Account		*_account;
  bool			_hasHandShake;
  time_t		_connectionTime;
  time_t		_lastPingTime;
  PacketFactory		_packet;

public:
  Client(unsigned int uid, Account *account = NULL);
  Client(const Client& copy);
  virtual ~Client();
  Client&		operator=(const Client& other);
  unsigned int		getUid() const;
  Account		*getAccount() const;
  void			setAccount(Account *account);
  PacketFactory		&getPacket();
  void			accountLogout();
  bool			hasHandShake() const;
  void			setHasHandShake(bool value);
  time_t		getConnectionTime() const;
  time_t		getLastPingTime() const;
  void			setLastPingTime(time_t timer);
  void			setIp(const std::string &ip);
  const std::string	&getIp() const;
};
