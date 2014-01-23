#pragma once

#include						<string>

class							Account;

class							IManagerUser
{
public:
  virtual ~IManagerUser() {}
  virtual void						init() = 0;
  virtual bool						createAccount(const std::string& login, const std::string& hash) = 0;
  virtual bool						deleteAccount(const std::string& login) = 0;
  virtual Account					*findAccount(const std::string& login) = 0;
  virtual Account					*login(const std::string& login, const std::string& hash) = 0;
  virtual const std::map<std::string, Account *>	&getListAccount() const = 0;
  virtual bool						call(const std::string& from, const std::string& to) = 0;
  virtual bool						hangUp(const std::string& from) = 0;
  virtual bool						answerCall(const std::string& from, const std::string& to, bool accept) = 0;
  virtual bool						chat(const std::string& from, const std::string& to) = 0;
  virtual bool						renameAccount(const std::string& login, const std::string& newLogin) = 0;
};
