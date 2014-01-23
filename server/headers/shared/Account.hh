#pragma once

#include <string>
#include <map>
#include <list>
#include "LibC.hh"
#include "Exception.h"

class			Account
{
public:
  enum			eStatus
    {
      AVAILABLE = 0,
      BUSY,
      AWAY,
      INVISIBLE,
      ENUM_COUNT
    };

  enum			eStatusCall
    {
      NONE,
      ISWAITING,
      ISCALLING
    };

private:
  std::string					_login;
  std::string					_hash;
  std::string					_state;
  Account::eStatus				_status;
  std::map<std::string, bool>			_friends;
  Account::eStatusCall				_statusCall;
  time_t					_timeoutCall;
  std::list<Account *>				_conversation;
  bool						_isConnected;

public:
  Account(const std::string& login, const std::string& hash);
  Account(const Account& copy);
  virtual ~Account();
  Account				&operator=(const Account& other);
  bool					changePasswd(const std::string& hash);
  const std::string			&getHash() const;
  const std::string			&getLogin() const;
  bool					setStatus(Account::eStatus status);
  Account::eStatus			getStatus() const;
  const std::string			&getState() const;
  bool					setState(const std::string& state);
  Account::eStatusCall			getStatusCall() const;
  bool					setStatusCall(Account::eStatusCall statusCall);
  bool					addFriend(const std::string& friends);
  bool					deleteFriends(const std::string& friends);
  bool					rename(const std::string& newLogin);
  bool					friendBlock(const std::string& friends, bool block);
  std::list<std::string>		getListFriends() const;
  bool					renameFriends(const std::string& oldLogin, const std::string& newLogin);
  bool					isBlocked(const std::string& friends);
  std::list<Account *>			&getConversation();
  std::list<Account *>::iterator	findAccountConversation(const std::string& login);
  const std::map<std::string, bool>	&getFriends() const;
  void					setConnected(bool value);
  bool					isConnected() const;
  time_t				getTimeOutCall() const;
  void					setTimeOutCall(time_t timer);
};
