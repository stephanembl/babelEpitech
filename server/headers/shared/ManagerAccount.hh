#pragma once

#include "IManagerUser.hh"
#include "ISerialize.h"
#include "Account.hh"

class						ManagerAccount : public IManagerUser
{
private:
  std::map<std::string, Account *>		_accounts;
  ISerialize					*_serialize;

public:
  ManagerAccount();
  ManagerAccount(const ManagerAccount& copy);
  virtual ~ManagerAccount();
  ManagerAccount&				operator=(const ManagerAccount& other);
  void						init();
  bool						createAccount(const std::string& login, const std::string& hash);
  bool						deleteAccount(const std::string& login);
  Account					*findAccount(const std::string& login);
  Account					*login(const std::string& login, const std::string& hash);
  const std::map<std::string, Account *>	&getListAccount() const;
  bool						call(const std::string& from, const std::string& to);
  bool						hangUp(const std::string& from);
  bool						answerCall(const std::string& from, const std::string& to, bool accept);
  bool						chat(const std::string& from, const std::string& to);
  bool						renameAccount(const std::string& login, const std::string& newLogin);
};
